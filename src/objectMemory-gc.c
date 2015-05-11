#include <string.h> /* memcpy */
#include "common.h"
#include "constants.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-remembered.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "vm.h"


// Generation scavenging is used for garbage collection. Objects are separated
// into two generations: young ones and old ones. Young objects have age
// associated to them, new objects start at age zero. After the object
// survives GC_TENURE_AGE garbage collection cycles, they are considered to be
// long-lived and are tenured/promoted to the old generation. While garbage
// collecting the young generation, all reachable objects are copied into the
// survivor half of the young space, pointers to them are updated and the rest
// of the new generation which was not copied is thrown away as garbage. Then
// the halves are switched.
// 
// Garbage collection in the old generation is run much less often,
// a mark&sweep algorithm is used.


#define GC_TENURE_AGE 5


OBJ newSpace;
OBJ survivorSpace;

OBJ newSpaceEnd;
OBJ survivorSpaceEnd;

OBJ nextFreeNewObject;
OBJ nextFreeSurvivorObject;


void garbageCollectNewSpace() {
	// roots of reachability:
	//  - remembered set
	//  - VM registers (currently active context, top context, default exception handler context, currently executed script)
	//  - global dictionary (Smalltalk)
	
	processRememberedSet();
	processVmRegisters();
	if(!processObjectPointers(OBJECT_SMALLTALK_OP)) {
		processObjectPointers(fetchNamedPointer(OBJECT_SMALLTALK_OP, DICTIONARY_VALUE_ARRAY));
	}
	
	switchNewAndSurvivorSpace();
}

void switchNewAndSurvivorSpace() {
	OBJ tmpSurvivorSpace = survivorSpace;
	OBJ tmpSurvivorSpaceEnd = survivorSpaceEnd;
	OBJ tmpNextFreeSurvivorObject = nextFreeSurvivorObject;
	
	nextFreeSurvivorObject = survivorSpace = newSpace;
	survivorSpaceEnd = newSpaceEnd;
	
	newSpace = tmpSurvivorSpace;
	newSpaceEnd = tmpSurvivorSpaceEnd;
	nextFreeNewObject = tmpNextFreeSurvivorObject;
}

int processNewObject(OP op) {
	if(!isNewObject(op)) {
		// the object is not in new space => nothing to do
		return 0;
	}
	
	// if the OP stored in the object itself is different,
	// it means that it has already been copied
	if(op != fetchObjectPointer(op)) {
		return ALREADY_COPIED;
	}
	
	OP newOp = copyObjectToSurvivorSpace(op);
	
	// indicate that the object has already been copied
	storeFieldOf(op, OBJECT_OP_FIELD, newOp);
	
	int copied = 1 + processObjectPointers(newOp); // 1 = the object `op` itself
	
	if(isOldObject(newOp)) {
		checkAndAddToRemembered(newOp);
	}
	
	return copied;
}

int processObjectPointers(OP op) {
	int copied = 0;
	int result;
	OP valueOp;
	
	int namedPointerSize = fetchNamedPointerLength(op);
	int indexablePointerSize;
	
	if(fetchClass(op) == METHODCONTEXT_OP) {
		// in contexts we need to walk through objects on the stack only up to the stack pointer,
		// the other objects have been popped and should not be reachable anymore (althouth the pointers
		// have not been physically removed)
		indexablePointerSize = fetchNamedSmallInteger(op, METHODCONTEXT_STACK_POINTER);
	}
	else {
		indexablePointerSize = fetchIndexablePointerLength(op);
	}
	
	for(int index = 0; index < namedPointerSize; ++index) {
		valueOp = fetchNamedPointer(op, index);
		result = processNewObject(valueOp);
		
		if(result != 0) {
			// the object has been copied, update the pointer
			valueOp = fetchObjectPointer(valueOp);
			storeNamedPointer(op, index, valueOp);
		}
		if(result == ALREADY_COPIED) {
			result = isYoungObject(valueOp) ? 1 : 0;
		}
		
		copied += result;
	}
	for(int index = 0; index < indexablePointerSize; ++index) {
		valueOp = fetchIndexablePointer(op, index);
		result = processNewObject(valueOp);
		
		if(result != 0) {
			// the object has been copied, update the pointer
			valueOp = fetchObjectPointer(valueOp);
			storeIndexablePointer(op, index, valueOp);
		}
		if(result == ALREADY_COPIED) {
			result = isYoungObject(valueOp) ? 1 : 0;
		}
		
		copied += result;
	}
	
	return copied;
}

OP copyObjectToSurvivorSpace(OP op) {
	if(incrementObjectAge(op) >= GC_TENURE_AGE) {
		return copyObjectToOldSpace(op);
	}
	
	int size = fetchSize(op);
	
	OBJ survivor = allocateSurvivorObjectOfSize(size);
	memcpy(survivor, objectFor(op), size * sizeof(*survivor));
	
	OP survivorOp = objectPointerFor(survivor);
	storeFieldOf(survivorOp, OBJECT_OP_FIELD, survivorOp);
	
	return survivorOp;
}

OP copyObjectToOldSpace(OP op) {
	int size = fetchSize(op);
	
	OBJ old = allocateOldObjectOfSize(size);
	memcpy(old, objectFor(op), size * sizeof(*old));
	
	OP oldOp = objectPointerFor(old);
	storeFieldOf(oldOp, OBJECT_OP_FIELD, oldOp);
	
	storeNextRemembered(oldOp, NOT_REMEMBERED);
	
	return oldOp;
}

int fetchObjectAge(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	return fetchFieldOf(op, OBJECT_AGE_FIELD);
}

int incrementObjectAge(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	int newValue = fetchFieldOf(op, OBJECT_AGE_FIELD) + 1;
	storeFieldOf(op, OBJECT_AGE_FIELD, newValue);
	
	return newValue;
}
