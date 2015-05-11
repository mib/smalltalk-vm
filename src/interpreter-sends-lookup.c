#include <stdlib.h> /* calloc, free */
#include <string.h> /* memcpy, memset */
#include "common.h"
#include "constants.h"
#include "interpreter-sends-lookup.h"
#include "objectMemory-objects.h"


OP * methodLookupClassCache;
int methodLookupClassCacheEntries = METHOD_LOOKUP_CACHE_INITIAL_CLASSES;
int methodLookupClassCacheSize = METHOD_LOOKUP_CACHE_INITIAL_CLASSES * 2;

OP * methodLookupSelectorCache;
int methodLookupSelectorCacheFreeSpace = 0;
int methodLookupSelectorCacheSize = METHOD_LOOKUP_CACHE_INITIAL_CLASSES * METHOD_LOOKUP_CACHE_ENTRY_SIZE * 2;


OP lookupMethod(OP selector, OP class) {
	if(class == OBJECT_NIL_OP) {
		return OBJECT_NIL_OP;
	}
	
	OP methods = fetchNamedPointer(class, BEHAVIOR_METHODS);
	OP keyArray = fetchNamedPointer(methods, DICTIONARY_KEY_ARRAY);
	int keyCount = fetchIndexablePointerLength(keyArray);
	
	for(int index = 0; index < keyCount; ++index) {
		if(fetchIndexablePointer(keyArray, index) == selector) {
			OP valueArray = fetchNamedPointer(methods, DICTIONARY_VALUE_ARRAY);
			return fetchIndexablePointer(valueArray, index);
		}
	}
	
	OP superclass = fetchNamedPointer(class, BEHAVIOR_SUPERCLASS);
	return lookupMethodInCache(selector, superclass);
}

void allocateMethodLookupCache() {
	// calloc will set the memory to zero (equals to OBJECT_NIL_OP)
	methodLookupClassCache = (OP *) calloc(methodLookupClassCacheSize, sizeof(*methodLookupClassCache));
	methodLookupSelectorCache = (OP *) calloc(methodLookupSelectorCacheSize, sizeof(*methodLookupSelectorCache));
}

void deallocateMethodLookupCache() {
	free(methodLookupClassCache);
	free(methodLookupSelectorCache);
}

void enlargeMethodLookupCache() {
	OP * oldMethodLookupClassCache = methodLookupClassCache;
	int oldMethodLookupClassCacheEntries = methodLookupClassCacheEntries;
	OP * oldMethodLookupSelectorCache = methodLookupSelectorCache;
	int oldMethodLookupSelectorCacheSize = methodLookupSelectorCacheSize;
	
	methodLookupClassCacheEntries <<= 1;
	methodLookupClassCacheSize <<= 1;
	methodLookupSelectorCacheSize <<= 1;
	
	allocateMethodLookupCache();
	
	memcpy(methodLookupSelectorCache, oldMethodLookupSelectorCache, sizeof(*methodLookupSelectorCache) * oldMethodLookupSelectorCacheSize);
	
	int classHash;
	OP class;
	
	// rehash the table
	for(int index = 0; index < oldMethodLookupClassCacheEntries; index += 2) {
		class = oldMethodLookupClassCache[index];
		
		if(class == OBJECT_NIL_OP) {
			continue;
		}
		
		classHash = hashClassForLookupCache(class);
		
		methodLookupClassCache[classHash] = class;
		methodLookupClassCache[classHash + 1] = oldMethodLookupClassCache[index + 1];
	}
	
	free(oldMethodLookupClassCache);
	free(oldMethodLookupSelectorCache);
}

// used by new space GC
void processMethodLookupCache() {
	// not needed, all classes, methods and symbols are allocated directly in the old space;
	// if they contain pointers to new objects, they are already in the remembered set
}

// used by old space GC (the objects might be moved when compacting)
void updateMethodLookupCache() {
	// it might be faster to update/rehash the table instead of wiping
	memset(methodLookupClassCache, 0, sizeof(*methodLookupClassCache) * methodLookupClassCacheSize);
	memset(methodLookupSelectorCache, 0, sizeof(*methodLookupSelectorCache) * methodLookupSelectorCacheSize);
}

// used when creating a new method
void removeSelectorFromMethodLookupCache(OP selector) {
	for(int index = 0; index < methodLookupSelectorCacheFreeSpace; index += 2) {
		if(methodLookupSelectorCache[index] == selector) {
			methodLookupSelectorCache[index] = OBJECT_NIL_OP;
		}
	}
}

// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

int hashClassForLookupCache(OP class) {
	return (class % methodLookupClassCacheEntries) << 1;
}

void addMethodToLookupCache(OP selector, OP method, int position) {
	methodLookupSelectorCache[position] = selector;
	methodLookupSelectorCache[position + 1] = method;
}

#endif

OP lookupMethodInCache(OP selector, OP class) {
	int classPosition = hashClassForLookupCache(class);
	
	// the outer `if` might seem useless but execution is actually a little bit faster this way
	// since the first access is a hit most of the time
	if(methodLookupClassCache[classPosition] != class) {
		while(methodLookupClassCache[classPosition] != class) {
			if(methodLookupClassCache[classPosition] == OBJECT_NIL_OP) {
				if((((methodLookupSelectorCacheSize - methodLookupSelectorCacheFreeSpace) / METHOD_LOOKUP_CACHE_ENTRY_SIZE) >> 1) < METHOD_LOOKUP_CACHE_FREE_SPACE) {
					enlargeMethodLookupCache();
				}
				
				classPosition = hashClassForLookupCache(class);
				
				methodLookupClassCache[classPosition] = class;
				methodLookupClassCache[classPosition + 1] = methodLookupSelectorCacheFreeSpace;
				methodLookupSelectorCacheFreeSpace += METHOD_LOOKUP_CACHE_ENTRY_SIZE * 2;
				
				OP method = lookupMethod(selector, class);
				
				if(method != OBJECT_NIL_OP) {
					addMethodToLookupCache(selector, method, methodLookupClassCache[classPosition + 1]);
				}
				
				return method;
			}
			if((classPosition += 2) >= methodLookupClassCacheSize) {
				classPosition = 0;
			}
		}
	}
	
	int selectorCacheOffset = methodLookupClassCache[classPosition + 1];
	OP * selectorCacheStart;
	OP * selectorCachePosition = selectorCacheStart = methodLookupSelectorCache + selectorCacheOffset;
	OP * selectorCacheEnd = selectorCachePosition + METHOD_LOOKUP_CACHE_ENTRY_SIZE * 2;
	OP entry;
	
	for(; selectorCachePosition < selectorCacheEnd; selectorCachePosition += 2) {
		if(*selectorCachePosition == selector) {
			return *(selectorCachePosition + 1);
		}
	}
	
	// not found in cache, find the first empty space to store it
	for(selectorCachePosition = selectorCacheStart; selectorCachePosition < selectorCacheEnd; selectorCachePosition += 2) {
		if(*selectorCachePosition == OBJECT_NIL_OP) {
			break;
		}
	}
	
	if(selectorCachePosition < selectorCacheEnd) {
		selectorCacheOffset += selectorCachePosition - selectorCacheStart;
	}
	
	OP method = lookupMethod(selector, class);
	
	if(method != OBJECT_NIL_OP) {
		addMethodToLookupCache(selector, method, selectorCacheOffset);
	}
	
	return method;
}
