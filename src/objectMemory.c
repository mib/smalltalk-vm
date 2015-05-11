#include <stdlib.h> /* malloc, free */
#include "common.h"
#include "constants.h"
#include "objectMemory.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-old.h"
#include "objectMemory-smallIntegers.h"


// Object memory is allocated as young space (smaller) and old space (bigger).
// Young space is divided into two halves (new and survivor), of which only one
// (new) is in use at any given moment. After each garbage collection run the
// halves are switched.
// 
// Object pointers (OP) are offsets from the beginning of the space and are
// tagged according to the space to which they belong:
//   - xxxx...xxx1 => small integer (its value is determined from the OP,
//                                   it is not stored in memory at all)
//   - xxxx...xx10 => object in the young space starting at youngSpace + OP>>2
//   - xxxx...xx00 => object in the old space starting at oldSpace + OP>>2
// 
// New objects are allocated as young, except for objects contained in the
// image that are mostly classes and methods and are therefore long-lived or
// permanent. Also, any new symbols, classes and methods are allocated directly
// in the old space because they are likely to be permanent. Objects that are
// too big are also allocated in the old space.


OBJ youngSpace;
OBJ newSpace;
OBJ survivorSpace;
OBJ oldSpace;

OBJ youngSpaceEnd;
OBJ newSpaceEnd;
OBJ survivorSpaceEnd;
OBJ oldSpaceEnd;
OBJ oldSpaceSafeEnd; // old space GC is triggered when this point is reached
                     // since new space GC is triggered as well and it might
                     // move (tenure) some objects to the old space

// next available position for allocating new objects
OBJ nextFreeNewObject;
OBJ nextFreeSurvivorObject;
OBJ nextFreeOldObject;

int gcInProgress;

long totalMemoryAllocated;
long totalObjectsAllocated;


void allocateObjectMemory() {
	youngSpace = (OBJ) malloc(DEFAULT_YOUNG_SPACE_SIZE * sizeof(OP));
	youngSpaceEnd = youngSpace + DEFAULT_YOUNG_SPACE_SIZE;
	
	newSpace = nextFreeNewObject = youngSpace;
	newSpaceEnd = newSpace + DEFAULT_YOUNG_SPACE_SIZE / 2;
	
	survivorSpace = nextFreeSurvivorObject = newSpaceEnd;
	survivorSpaceEnd = youngSpaceEnd;
	
	oldSpace = nextFreeOldObject = (OBJ) malloc(DEFAULT_OLD_SPACE_SIZE * sizeof(OP));
	oldSpaceEnd = oldSpace + DEFAULT_OLD_SPACE_SIZE;
	
	// safe upper bound for old space allocations; if a GC needs to be triggered
	// for the old space, the young space needs to be GC'd as well and that might
	// tenure some objects to the old space
	oldSpaceSafeEnd = oldSpaceEnd - DEFAULT_YOUNG_SPACE_SIZE / 2;
}

void deallocateObjectMemory() {
	free(youngSpace);
	free(oldSpace);
}

void noSpaceLeftInOldSpace() {
	error("Out of space on the heap.");
}


/* allocating and fetching objects */

OBJ allocateNewObjectOfSize(int size) {
	++totalObjectsAllocated;
	totalMemoryAllocated += size;
	
	// garbage collection might not free enough space in the new space
	// so we repeat it until it does - when tenure is triggered
	while(!hasNewSpaceAtLeast(size)) {
		garbageCollectNewSpace();
	}
	
	OBJ object = nextFreeNewObject;
	nextFreeNewObject += size;
	
	return object;
}

// optimization: replaced with a macro
#ifndef OPTIMIZED_MACROS

int hasNewSpaceAtLeast(int size) {
	return (nextFreeNewObject + size < newSpaceEnd);
}

#endif

OBJ allocateSurvivorObjectOfSize(int size) {
	OBJ object = nextFreeSurvivorObject;
	nextFreeSurvivorObject += size;
	
	return object;
}

OBJ allocateOldObjectOfSize(int size) {
	++totalObjectsAllocated;
	totalMemoryAllocated += size;
	
	// we need to check if the GC isn't already running since the new space GC
	// allocates old objects when tenuring and will trigger the old space GC
	// afterwards if needed
	if(nextFreeOldObject + size >= oldSpaceSafeEnd && !gcInProgress) {
		garbageCollectOldSpace();
		
		if(nextFreeOldObject + size >= oldSpaceSafeEnd) {
			noSpaceLeftInOldSpace();
		}
	}
	
	OBJ object = nextFreeOldObject;
	nextFreeOldObject += size;
	
	return object;
}

// used only when loading the image file
OBJ allocateOldObjectOfSizeAtPosition(int size, int position) {
	++totalObjectsAllocated;
	totalMemoryAllocated += size;
	
	OBJ object = oldSpace + position;
	
	if(nextFreeOldObject < object + size) {
		nextFreeOldObject = object + size;
	}
	
	return object;
}


// optimization: replaced with a macro
#ifndef OPTIMIZED_MACROS

int isNewObject(OP op) {
	if(isSmallIntegerObject(op)) {
		return 0;
	}
	
	OBJ object = objectFor(op);
	
	return object >= newSpace && object < newSpaceEnd;
}
int isSurvivorObject(OP op) {
	if(isSmallIntegerObject(op)) {
		return 0;
	}
	
	OBJ object = objectFor(op);
	
	return object >= survivorSpace && object < survivorSpaceEnd;
}
int isYoungObject(OP op) {
	return (op & 3) == 2;
}
int isOldObject(OP op) {
	return (op & 3) == 0;
}

#endif

// optimization: replaced with a macro
#ifndef OPTIMIZED_MACROS

OBJ objectFor(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	if((op & 3) == 0) {
		return oldSpace + (op >> 2);
	}
	
	return youngSpace + (op >> 2);
}

OP objectPointerFor(OBJ object) {
	if(object >= oldSpace && object < oldSpaceEnd) {
		return (object - oldSpace) << 2;
	}
	
	return ((object - youngSpace) << 2) | 2;
}

#endif
