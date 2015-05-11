#include <stdlib.h> /* malloc, free */
#include "common.h"
#include "constants.h"
#include "objectMemory.h"
#include "objectMemory-gc.h"
#include "objectMemory-smallIntegers.h"


// Object memory is allocated as young space (small) and old space (big).
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
// permanent.


OBJ youngSpace;
OBJ newSpace;
OBJ survivorSpace;
OBJ oldSpace;

OBJ youngSpaceEnd;
OBJ newSpaceEnd;
OBJ survivorSpaceEnd;
OBJ oldSpaceEnd;

OBJ nextFreeNewObject;
OBJ nextFreeSurvivorObject;
OBJ nextFreeOldObject;


void allocateObjectMemory() {
	youngSpace = (OBJ) malloc(DEFAULT_YOUNG_SPACE_SIZE * sizeof(OP));
	youngSpaceEnd = youngSpace + DEFAULT_YOUNG_SPACE_SIZE;
	
	newSpace = nextFreeNewObject = youngSpace;
	newSpaceEnd = newSpace + DEFAULT_YOUNG_SPACE_SIZE / 2;
	
	survivorSpace = nextFreeSurvivorObject = newSpaceEnd;
	survivorSpaceEnd = youngSpaceEnd;
	
	nextFreeOldObject = oldSpace = (OBJ) malloc(DEFAULT_OLD_SPACE_SIZE * sizeof(OP));
	oldSpaceEnd = oldSpace + DEFAULT_OLD_SPACE_SIZE;
}

void deallocateObjectMemory() {
	free(youngSpace);
	free(oldSpace);
}


/* allocating and fetching objects */

OBJ allocateNewObjectOfSize(int size) {
	while(!hasNewSpaceAtLeast(size));
	
	OBJ object = nextFreeNewObject;
	nextFreeNewObject += size;
	
	return object;
}

int hasNewSpaceAtLeast(int size) {
	if(nextFreeNewObject + size >= newSpaceEnd) {
		// garbage collection might not free enough space in the new space
		// so we repeat it until it does - when tenure is triggered
		garbageCollectNewSpace();
		
		return 0;
	}
	
	return 1;
}

OBJ allocateSurvivorObjectOfSize(int size) {
	OBJ object = nextFreeSurvivorObject;
	nextFreeSurvivorObject += size;
	
	return object;
}

OBJ allocateOldObjectOfSize(int size) {
	if(nextFreeOldObject + size >= oldSpaceEnd) {
		// TODO: trigger GC of old space
		error("Out of space on the heap.");
	}
	
	OBJ object = nextFreeOldObject;
	nextFreeOldObject += size;
	
	return object;
}

OBJ allocateOldObjectOfSizeAtPosition(int size, int position) {
	OBJ object = oldSpace + position;
	
	if(nextFreeOldObject < object + size) {
		nextFreeOldObject = object + size;
	}
	
	return object;
}

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
	if(object >= youngSpace && object < youngSpaceEnd) {
		return ((object - youngSpace) << 2) | 2;
	}
}
