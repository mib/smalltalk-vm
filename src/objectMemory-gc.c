#include <string.h> /* memcpy */
#include "common.h"
#include "constants.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-old.h"
#include "objectMemory-gc-remembered.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "context-stack-active.h"
#include "interpreter-sends.h"
#include "interpreter-sends-lookup.h"
#include "vm.h"
#include "interpreter.h"


// Generation scavenging is used for garbage collection. Objects are separated
// into two generations: young ones and old ones. Young objects have age
// associated to them, new objects start at age zero. After the object
// survives GC_TENURE_AGE garbage collection cycles, they are considered to be
// long-lived and are tenured/promoted to the old generation. Baker's
// semi-space algorithm is used to collect the young generation. Objects are
// allocated in the new half of the young space. During garbage collection all
// reachable objects are copied into the survivor half of the young space,
// pointers to them are updated and the rest of the new generation which was
// not copied is thrown away as garbage. Then the halves are switched.
// 
// Garbage collection in the old generation is performed much less often, since
// most garbage is collected within young objects. The number of objects that
// get tenured to the old generation is very low.
// A compacting mark&sweep algorithm is used for the old generation.


#define GC_TENURE_AGE 5
#define GC_LARGE_OBJECT_TENURE_AGE 3
#define GC_LARGE_OBJECT_SIZE 200
#define GC_VERY_LARGE_OBJECT_TENURE_AGE 1
#define GC_VERY_LARGE_OBJECT_SIZE 1000


OBJ newSpace;
OBJ survivorSpace;
OBJ oldSpace;

OBJ newSpaceEnd;
OBJ survivorSpaceEnd;
OBJ oldSpaceEnd;
OBJ oldSpaceSafeEnd;

OBJ nextFreeNewObject;
OBJ nextFreeSurvivorObject;
OBJ nextFreeOldObject;

OP firstRememberedObject;

OBJ survivorProcessingPosition;


long objectSizeMoved;

int gcCount = 0;
double gcPercentMin = 100.0;
double gcPercentSum = 0.0;
long gcObjectsMovedToOld = 0L;

int gcInProgress = 0;


void garbageCollectNewSpace() {
	++gcCount;
	
	int wasGcInProgress = gcInProgress;
	gcInProgress = 1;
	
	objectSizeMoved = 0L;
	
	activeContextStackSave();
	
	// roots of reachability:
	//  – the remembered set
	//  – VM registers (currently active context, top context, default exception handler context, currently executed script)
	//  – interpreter registers
	//  – global dictionary (Smalltalk) – to process object stored in global variables not reachable from elsewhere
	
	processRememberedSet();
	processVmRegisters();
	processInterpreterRegisters();
	processInterpreterCallRegisters();
	// processMethodLookupCache(); // not necessary: symbols, classes and methods are allocated in the old space
	if(!processObjectPointers(OBJECT_SMALLTALK_OP)) {
		processObjectPointers(fetchNamedPointer(OBJECT_SMALLTALK_OP, DICTIONARY_VALUE_ARRAY));
	}
	
	// walking through and updating pointers is done separately using a second pointer
	// to avoid recursion
	processAllPointers();
	
	// switch the two halves of the young space, reclaiming garbage in the former new half
	switchNewAndSurvivorSpace();
	
	// compute statistics
	double percentReclaimed = (1 - (double) objectSizeMoved / DEFAULT_YOUNG_SPACE_SIZE) * 100;
	if(percentReclaimed < gcPercentMin) {
		gcPercentMin = percentReclaimed;
	}
	gcPercentSum += percentReclaimed;
	
	// the GC has moved some objects to the old space and the old space itself
	// now needs to be collected
	if(!wasGcInProgress && nextFreeOldObject >= oldSpaceSafeEnd) {
		garbageCollectOldSpace();
		
		if(nextFreeOldObject >= oldSpaceSafeEnd) {
			noSpaceLeftInOldSpace();
		}
	}
	if(!wasGcInProgress) {
		gcInProgress = 0;
	}
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
		// the object is not in the new space => nothing to do
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
	
	if(isOldObject(newOp)) {
		processObjectPointers(newOp);
	}
	
	return 1;
}

void processAllPointers() {
	survivorProcessingPosition = survivorSpace;
	
	OP op;
	
	// when the pointers meet, all objects have been copied and walked through
	while(survivorProcessingPosition < nextFreeSurvivorObject) {
		op = objectPointerFor(survivorProcessingPosition);
		processObjectPointers(op);
		
		// object size is the first field
		survivorProcessingPosition += *survivorProcessingPosition;
	}
}

int processObjectPointers(OP op) {
	int copied = 0;
	int result;
	OP valueOp;
	
	int namedPointerSize = fetchNamedPointerLength(op);
	int indexablePointerSize;
	
	if(fetchClass(op) == METHODCONTEXT_OP) {
		// in contexts we need to walk through objects on the stack only up to the stack pointer,
		// the other objects have been popped and should not be reachable anymore (although the
		// pointers have not been physically removed)
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
	int age = incrementObjectAge(op);
	
	// the object has survived GC_TENURE_AGE cycles, tenure it to the old space
	if(age >= GC_TENURE_AGE) {
		return copyObjectToOldSpace(op);
	}
	
	int size = fetchSize(op);
	
	// large objects get tenured more quickly
	if((size >= GC_LARGE_OBJECT_SIZE && age >= GC_LARGE_OBJECT_TENURE_AGE) ||
	   (size >= GC_VERY_LARGE_OBJECT_SIZE && age >= GC_VERY_LARGE_OBJECT_TENURE_AGE)) {
		return copyObjectToOldSpace(op);
	}
	
	objectSizeMoved += size;
	
	OBJ survivor = allocateSurvivorObjectOfSize(size);
	memcpy(survivor, objectFor(op), size * sizeof(*survivor));
	
	OP survivorOp = objectPointerFor(survivor);
	storeFieldOf(survivorOp, OBJECT_OP_FIELD, survivorOp);
	
	return survivorOp;
}

OP copyObjectToOldSpace(OP op) {
	++gcObjectsMovedToOld;
	
	int size = fetchSize(op);
	
	objectSizeMoved += size;
	
	OBJ old = allocateOldObjectOfSize(size);
	memcpy(old, objectFor(op), size * sizeof(*old));
	
	OP oldOp = objectPointerFor(old);
	storeFieldOf(oldOp, OBJECT_OP_FIELD, oldOp);
	
	// we could check if the object actually needs to be in the remembered set
	// but that would require an extra walk through its pointers;
	// it will be walked through the next time we process the remembered set anyway
	// (and removed if it does not belong there)
	storeNextRemembered(oldOp, NOT_REMEMBERED);
	addToRemembered(oldOp);
	
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
