#include <string.h> /* memcpy, memmove */
#include <stdlib.h> /* malloc, calloc, free */
#include "common.h"
#include "constants.h"
#include "objectMemory-gc-old.h"
#include "objectMemory-gc.h"
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

int gcOldCount = 0;
double gcOldPercentMin = 100.0;
double gcOldPercentSum = 0.0;

int gcInProgress;

OP * breakTable = NULL;
int breakTableSize;
int breakTablePosition;

char * markTable;
char * markTableNew;
char * markTableOld;

void garbageCollectOldSpace() {
	++gcOldCount;
	OBJ nextFreeOldObjectBefore = nextFreeOldObject;
	
	// if gcInProgress == 1, then the new space GC triggered the old space GC;
	// if not, trigger the new space GC
	if(!gcInProgress) {
		gcInProgress = 1;
		garbageCollectNewSpace();
	}
	
	breakTableSize = 10000;
	allocateBreakTable();
	allocateMarkTable();
	
	// the active context's stack pointer should also be saved
	// but the new space GC already does that
	
	markPhase();
	compactPhase(oldSpace, nextFreeOldObject);
	
	updateOldObjectsPhase(oldSpace, nextFreeOldObject);
	updateYoungObjectsPhase(newSpace, nextFreeNewObject);
	updateRegistersPhase();
	
	deallocateBreakTable();
	deallocateMarkTable();
	
	gcInProgress = 0;
	
	if((1 - (double) (nextFreeOldObjectBefore - nextFreeOldObject) / DEFAULT_OLD_SPACE_SIZE) * 100 < gcOldPercentMin) {
		gcOldPercentMin = (1 - (double) (nextFreeOldObjectBefore - nextFreeOldObject) / DEFAULT_OLD_SPACE_SIZE) * 100;
	}
	gcOldPercentSum += (1 - (double) (nextFreeOldObjectBefore - nextFreeOldObject) / DEFAULT_OLD_SPACE_SIZE) * 100;
}

void markPhase() {
	markAndProcessVmRegisters();
	markAndProcessInterpreterRegisters();
	markAndProcessObjectPointer(OBJECT_SMALLTALK_OP);
}

void allocateMarkTable() {
	// store 8 bits in one char
	int markTableNewSize = (DEFAULT_YOUNG_SPACE_SIZE / 2 + 7) / 8;
	int markTableOldSize = (DEFAULT_OLD_SPACE_SIZE + 7) / 8;
	
	// calloc will set the memory to zero
	markTable = (char *) calloc(markTableNewSize + markTableOldSize, sizeof(*markTable));
	
	markTableNew = markTable;
	markTableOld = markTable + markTableNewSize;
}

void deallocateMarkTable() {
	free(markTable);
}

void markAndProcessObjectPointer(OP op) {
	if(!isSmallIntegerObject(op)) {
		markAndProcessObject(objectFor(op));
	}
}

void markAndProcessObject(OBJ object) {
	if(isObjectMarked(object)) {
		return;
	}
	
	if(object >= oldSpace && object < oldSpaceEnd) {
		markOldObject(object);
	}
	else {
		markNewObject(object);
	}
	
	markAndProcessObjectPointer(*(object + OBJECT_CLASS_FIELD));
	
	int field = *(object + OBJECT_NAMED_POINTER_FIRST_FIELD);
	int fieldEnd;
	
	if(*(object + OBJECT_CLASS_FIELD) == METHODCONTEXT_OP) {
		OP stackPointerOp = *(object + *(object + OBJECT_NAMED_POINTER_FIRST_FIELD) + METHODCONTEXT_STACK_POINTER);
		fieldEnd = *(object + OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + smallIntegerValueOf(stackPointerOp);
	}
	else {
		fieldEnd = *(object + OBJECT_INDEXABLE_BYTE_FIRST_FIELD);
	}
	
	for(; field < fieldEnd; ++field) {
		markAndProcessObjectPointer(*(object + field));
	}
}

int isObjectMarked(OBJ object) {
	if(object >= oldSpace && object < oldSpaceEnd) {
		return isOldObjectMarked(object);
	}
	else {
		return isNewObjectMarked(object);
	}
}

void markOldObject(OBJ object) {
	int offset = object - oldSpace;
	*(markTableOld + offset / 8) |= 1 << (offset % 8);
}

void markNewObject(OBJ object) {
	int offset = object - newSpace;
	*(markTableNew + offset / 8) |= 1 << (offset % 8);
}

int isOldObjectMarked(OBJ object) {
	int offset = object - oldSpace;
	return (*(markTableOld + offset / 8)) & (1 << (offset % 8));
}

int isNewObjectMarked(OBJ object) {
	int offset = object - newSpace;
	return (*(markTableNew + offset / 8)) & (1 << (offset % 8));
}

void compactPhase(OBJ spaceStart, OBJ spaceEnd) {
	breakTable[0] = objectPointerFor(spaceStart);
	breakTable[1] = 0;
	breakTablePosition = 2;
	
	OBJ position = spaceStart;
	nextFreeOldObject = spaceStart;
	OBJ chunkStart = NULL;
	int chunkSize;
	
	OP nextRemembered = NO_NEXT_REMEMBERED;
	
	while(position < spaceEnd) {
		// object size is the first field;
		// we need to save it now because the move may overwrite it
		int objectSize = *position;
		
		if(isOldObjectMarked(position)) {
			if(chunkStart == NULL) {
				chunkStart = position;
			}
			
			if(*(position + OBJECT_NEXT_REMEMBERED_INDEX_FIELD) != NOT_REMEMBERED) {
				*(position + OBJECT_NEXT_REMEMBERED_INDEX_FIELD) = nextRemembered;
				nextRemembered = objectPointerFor(position);
			}
		}
		else if(chunkStart != NULL) {
			chunkSize = position - chunkStart;
			
			if(chunkStart != nextFreeOldObject) {
				moveChunk(chunkStart, chunkSize, nextFreeOldObject);
			}
			
			nextFreeOldObject += chunkSize;
			chunkStart = NULL;
		}
		
		position += objectSize;
	}
	
	firstRememberedObject = nextRemembered;
	
	if(chunkStart != NULL) {
		chunkSize = position - chunkStart;
		moveChunk(chunkStart, chunkSize, nextFreeOldObject);
		nextFreeOldObject += chunkSize;
	}
}

void moveChunk(OBJ chunkStart, int chunkSize, OBJ targetPosition) {
	if(breakTablePosition >= breakTableSize) {
		enlargeBreakTable();
	}
	
	breakTable[breakTablePosition++] = objectPointerFor(chunkStart);
	breakTable[breakTablePosition++] = (targetPosition - chunkStart) << 2;
	
	// memmove is needed because the source area and the target area might overlap
	memmove(targetPosition, chunkStart, sizeof(*chunkStart) * chunkSize);
}

void allocateBreakTable() {
	breakTable = (OP *) malloc(breakTableSize * sizeof(*breakTable));
}

void deallocateBreakTable() {
	free(breakTable);
}

void enlargeBreakTable() {
	OP * oldBreakTable = breakTable;
	int oldBreakTableSize = breakTableSize;
	
	breakTableSize <<= 1;
	allocateBreakTable();
	
	memcpy(breakTable, oldBreakTable, sizeof(*oldBreakTable) * oldBreakTableSize);
	
	free(oldBreakTable);
}

void updateOldObjectsPhase(OBJ spaceStart, OBJ spaceEnd) {
	OBJ position = spaceStart;
	
	while(position < spaceEnd) {
		updateOldObject(position);
		
		// object size is the first field
		position += *position;
	}
	
	updatePointer(&firstRememberedObject);
}

void updateYoungObjectsPhase(OBJ spaceStart, OBJ spaceEnd) {
	OBJ position = spaceStart;
	
	while(position < spaceEnd) {
		updateYoungObject(position);
		
		// object size is the first field
		position += *position;
	}
}

void updateRegistersPhase() {
	updateVmRegisters();
	updateInterpreterRegisters();
	updateInterpreterCallRegisters();
	updateMethodLookupCache();
}

void updateOldObject(OBJ object) {
	*(object + OBJECT_OP_FIELD) = objectPointerFor(object);
	
	updatePointer(object + OBJECT_NEXT_REMEMBERED_INDEX_FIELD);
	
	updateYoungObject(object);
}

void updateYoungObject(OBJ object) {
	updatePointer(object + OBJECT_CLASS_FIELD);
	
	int field = *(object + OBJECT_NAMED_POINTER_FIRST_FIELD);
	int fieldEnd;
	
	if(*(object + OBJECT_CLASS_FIELD) == METHODCONTEXT_OP) {
		fieldEnd = *(object + OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + *(object + *(object + OBJECT_NAMED_POINTER_FIRST_FIELD) + METHODCONTEXT_STACK_POINTER);
	}
	else {
		fieldEnd = *(object + OBJECT_INDEXABLE_BYTE_FIRST_FIELD);
	}
	
	for(; field < fieldEnd; ++field) {
		updatePointer(object + field);
	}
}

void updatePointer(OBJ field) {
	OP pointer = *field;
	
	if(isOldObject(pointer)) {
		*field = pointer + differenceForPointer(pointer);
	}
}

int differenceForPointer(OP pointer) {
	int bottom = 0;
	int top = breakTablePosition / 2 - 1;
	int current;
	
	// binary search for the highest OP that is still less than or equal to the argument
	while(bottom != top) {
		current = (bottom + top + 1) / 2;
		if(breakTable[current * 2] <= pointer) {
			bottom = current;
		}
		else if(breakTable[current * 2] > pointer) {
			top = current - 1;
		}
	}
	
	return breakTable[bottom * 2 + 1];
}
