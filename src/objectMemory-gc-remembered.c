#include "common.h"
#include "constants.h"
#include "objectMemory-gc-remembered.h"
#include "objectMemory.h"
#include "objectMemory-gc.h"
#include "objectMemory-objects.h"


static OP firstRememberedObject = NOT_REMEMBERED;


void addToRemembered(OP op) {
	if(fetchNextRemembered(op) != NOT_REMEMBERED) {
		// the object is already included in the remembered set
		return;
	}
	
	storeNextRemembered(op, firstRememberedObject);
	firstRememberedObject = op;
}

void checkAndAddToRemembered(OP op) {
	if(isNewObject(op)) {
		return;
	}
	
	int namedPointerSize = fetchNamedPointerLength(op);
	int indexablePointerSize = fetchIndexablePointerLength(op);
	
	for(int index = 0; index < namedPointerSize; ++index) {
		if(isSurvivorObject(fetchNamedPointer(op, index))) {
			addToRemembered(op);
			return;
		}
	}
	for(int index = 0; index < indexablePointerSize; ++index) {
		if(isSurvivorObject(fetchIndexablePointer(op, index))) {
			addToRemembered(op);
			return;
		}
	}
	
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
	
	while(current != NOT_REMEMBERED) {
		newObjectPointerCount = processObjectPointers(current);
		
		next = fetchNextRemembered(current);
		
		if(!newObjectPointerCount) {
			removeFromRemembered(current);
		}
		
		current = next;
	}
}
