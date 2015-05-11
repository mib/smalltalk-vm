#include <string.h> /* memset */
#include "common.h"
#include "constants.h"
#include "objectMemory-objects.h"
#include "objectMemory-gc-remembered.h"
#include "objectMemory.h"
#include "objectMemory-smallIntegers.h"


OP fetchFieldOf(OP op, int fieldIndex) {
	return *(objectFor(op) + fieldIndex);
}

void storeFieldOf(OP op, int fieldIndex, OP valueOp) {
	*(objectFor(op) + fieldIndex) = valueOp;
}

OP fetchNamedPointer(OP op, int fieldIndex) {
	// TODO: check boundaries
	return fetchFieldOf(op, fetchFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD) + fieldIndex);
}

void storeNamedPointer(OP op, int fieldIndex, OP valueOp) {
	if(isNewObject(valueOp) && !isNewObject(op)) {
		addToRemembered(op);
	}
	
	// TODO: check boundaries
	storeFieldOf(op, fetchFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD) + fieldIndex, valueOp);
}

OP fetchIndexablePointer(OP op, int fieldIndex) {
	// TODO: check boundaries
	return fetchFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + fieldIndex);
}

void storeIndexablePointer(OP op, int fieldIndex, OP valueOp) {
	if(isNewObject(valueOp) && !isNewObject(op)) {
		addToRemembered(op);
	}
	
	// TODO: check boundaries
	storeFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + fieldIndex, valueOp);
}

int fetchIndexableByte(OP op, int fieldIndex) {
	// TODO: check boundaries
	unsigned int fourBytes = fetchFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_BYTE_FIRST_FIELD) + fieldIndex / 4);
	return (fourBytes >> ((3 - fieldIndex % 4) * 8)) & 0xff;
}

void storeIndexableByte(OP op, int fieldIndex, int value) {
	// TODO: check boundaries
	// 0 – 255
	int originalValue = fetchIndexableByte(op, fieldIndex);
	unsigned int fourBytes = fetchFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_BYTE_FIRST_FIELD) + fieldIndex / 4);
	
	// we need the other bytes to remain untouched so we compute a XOR difference between the old and the new byte and XOR it back into the four bytes
	value = originalValue ^ value;
	fourBytes ^= value << ((3 - fieldIndex % 4) * 8);
	
	storeFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_BYTE_FIRST_FIELD) + fieldIndex / 4, fourBytes);
}

OP fetchClass(OP op) {
	if(isSmallIntegerObject(op)) {
		return SMALLINTEGER_OP;
	}
	
	return fetchFieldOf(op, OBJECT_CLASS_FIELD);
}

int fetchNamedPointerLength(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	return fetchFieldOf(op, OBJECT_NAMED_POINTER_SIZE_FIELD);
}

int fetchIndexablePointerLength(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	return fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_SIZE_FIELD);
}

int fetchPointerLength(OP op) {
	return fetchNamedPointerLength(op) + fetchIndexablePointerLength(op);
}

int fetchIndexableByteLength(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	return fetchFieldOf(op, OBJECT_INDEXABLE_BYTE_SIZE_FIELD);
}

int fetchInstanceVariableSize(OP classDescription) {
	if(classDescription == OBJECT_NIL_OP) {
		return 0;
	}
	
	OP instanceVariables = fetchNamedPointer(classDescription, CLASSDESCRIPTION_INSTANCE_VARIABLES);
	OP superclass = fetchNamedPointer(classDescription, BEHAVIOR_SUPERCLASS);
	
	return fetchIndexablePointerLength(instanceVariables) + fetchInstanceVariableSize(superclass);
}

OP fetchObjectPointer(OP op) {
	if(isSmallIntegerObject(op)) {
		return op;
	}
	
	return fetchFieldOf(op, OBJECT_OP_FIELD);
}

int fetchSize(OP op) {
	if(isSmallIntegerObject(op)) {
		return OBJECT_HEADER_LENGTH;
	}
	
	return fetchFieldOf(op, OBJECT_SIZE_FIELD);
}

OP instantiateClass(OP class, int indexablePointerSize, int indexableByteSize) {
	return instantiateClassInSpace(class, indexablePointerSize, indexableByteSize, 0);
}

OP instantiateClassInOldSpace(OP class, int indexablePointerSize, int indexableByteSize) {
	return instantiateClassInSpace(class, indexablePointerSize, indexableByteSize, 1);
}

OP instantiateClassInSpace(OP class, int indexablePointerSize, int indexableByteSize, int forceOldSpace) {
	int namedPointerSize = fetchInstanceVariableSize(class);
	
	int fieldSize = namedPointerSize
	              + indexablePointerSize
	              + (indexableByteSize + 3) / 4; // storing 4 bytes in one integer => round to nearest 4 bytes
	
	int size = OBJECT_HEADER_LENGTH + fieldSize;
	
	OBJ object;
	int isOld;
	
	// when creating new classes (and metaclasses), allocate them directly in the old space
	// since they will probably live forever and (more importantly) the garbage collector
	// does not walk through or update class pointers of objects => classes cannot be moved
	if(!forceOldSpace && (fetchClass(class) == METACLASS_OP || fetchClass(class) == METACLASS_CLASS_OP)) {
		forceOldSpace = 1;
	}
	
	// if the object would be too big for the new space (more than half of its size), allocate it directly in the old space
	if(forceOldSpace || size >= DEFAULT_YOUNG_SPACE_SIZE / 2) {
		object = allocateOldObjectOfSize(size);
		isOld = 1;
	}
	else {
		object = allocateNewObjectOfSize(size);
		isOld = 0;
	}
	
	OP op = objectPointerFor(object);
	
	storeFieldOf(op, OBJECT_SIZE_FIELD, size);
	storeFieldOf(op, OBJECT_OP_FIELD, op);
	storeFieldOf(op, OBJECT_CLASS_FIELD, class);
	storeFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD, OBJECT_HEADER_LENGTH);
	storeFieldOf(op, OBJECT_NAMED_POINTER_SIZE_FIELD, namedPointerSize);
	storeFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD, OBJECT_HEADER_LENGTH + namedPointerSize);
	storeFieldOf(op, OBJECT_INDEXABLE_POINTER_SIZE_FIELD, indexablePointerSize);
	storeFieldOf(op, OBJECT_INDEXABLE_BYTE_FIRST_FIELD, OBJECT_HEADER_LENGTH + namedPointerSize + indexablePointerSize);
	storeFieldOf(op, OBJECT_INDEXABLE_BYTE_SIZE_FIELD, indexableByteSize);
	
	if(isOld) {
		storeNextRemembered(op, NOT_REMEMBERED);
	}
	else {
		storeFieldOf(op, OBJECT_AGE_FIELD, 0);
	}
	
	// fill memory to zero because there might be remainders of a previously garbage-collected object;
	// pointers will point to nil (op == 0), bytes will be zero
	memset(object + OBJECT_HEADER_LENGTH, 0, sizeof(OP) * fieldSize);
	
	return op;
}
