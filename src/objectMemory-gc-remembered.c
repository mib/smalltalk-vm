#include "common.h"
#include "constants.h"
#include "objectMemory-gc-remembered.h"
#include "objectMemory.h"
#include "objectMemory-gc.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


// Remembered objects are objects stored in the old space that have pointers
// to objects in the young space. They are stored as a single-linked list, each
// remembered object has the pointer to the next one stored in itself, variable
// firstRememberedObject stores the first one. Objects with NOT_REMEMBERED are
// not in the set. NO_NEXT_REMEMBERED means the end of the linked list.


OP firstRememberedObject = NO_NEXT_REMEMBERED;


void addToRemembered(OP op) {
	if(fetchNextRemembered(op) != NOT_REMEMBERED) {
		// the object is already included in the remembered set
		return;
	}
	
	storeNextRemembered(op, firstRememberedObject);
	
	firstRememberedObject = op;
}

void removeFromRemembered(OP op) {
	if(fetchNextRemembered(op) == NOT_REMEMBERED) {
		// the object is not included in the remembered set
		return;
	}
	
	OP nextRemembered = fetchNextRemembered(op);
	
	if(firstRememberedObject == op) {
		firstRememberedObject = nextRemembered;
	}
	else {
		OP prevRemembered = fetchPrevRemembered(op);
		storeNextRemembered(prevRemembered, nextRemembered);
	}
	
	storeNextRemembered(op, NOT_REMEMBERED);
}

OP fetchPrevRemembered(OP op) {
	OP current = firstRememberedObject;
	OP next;
	
	while((next = fetchNextRemembered(current)) != op) {
		current = next;
	}
	
	return current;
}
OP fetchNextRemembered(OP op) {
	return fetchFieldOf(op, OBJECT_NEXT_REMEMBERED_INDEX_FIELD);
}
void storeNextRemembered(OP op, OP valueOp) {
	storeFieldOf(op, OBJECT_NEXT_REMEMBERED_INDEX_FIELD, valueOp);
}

void processRememberedSet() {
	OP current = firstRememberedObject;
	OP next;
	int hasNext;
	int newObjectPointerCount;
	
	while(current != NOT_REMEMBERED && current != NO_NEXT_REMEMBERED) {
		newObjectPointerCount = processObjectPointers(current);
		
		next = fetchNextRemembered(current);
		
		if(!newObjectPointerCount) {
			removeFromRemembered(current);
		}
		
		current = next;
	}
}
