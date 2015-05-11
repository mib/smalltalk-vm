#include <string.h> /* memset */
#include "common.h"
#include "constants.h"
#include "objectMemory-objects.h"
#include "objectMemory-gc-remembered.h"
#include "objectMemory.h"
#include "objectMemory-smallIntegers.h"


// An object is represented in memory as an array of OPs/integers.
// 
// The structure of an object: (all indices are zero-based)
//   – header (size = 10):
//      0. total size of the object [integer] – including the header and this
//             field itself
//      1. object pointer [OP] – it is inconsistent with the actual position
//             in memory during GC, this field points to the new position
//             in case the object has been moved
//      2. (young objects) object's age [integer] – an object starts with zero,
//             it is incremented on every GC run (if it survives), if it
//             reaches a certain age (GC_TENURE_AGE), it is promoted to the old
//             space
//       / (old objects) pointer to the next remembered object – remembered
//             objects are a linked list, this field points to the next one;
//             if it is NOT_REMEMBERED, this object is not included in the
//             remembered set
//      3. class pointer [OP] – accessible by (anObject class)
//      4. index of the first named pointer [integer] – an offset from OP
//      5. named pointer count [integer]
//      6. index of the first indexable pointer [integer] – an offset from OP
//      7. indexable pointer count [integer] – accessible by (anObject size)
//      8. index of the first indexable byte [integer] – an offset from OP
//      9. indexable byte count [integer] – accessible by (anObject byteSize)
//   – body (size = total size − 10):
//      – named pointers – instance variables of an object accessed by name;
//             they are fixed and defined by the class
//      – indexable pointers – accessed by (anObject at: index); Smalltalk
//             indices are 1-based, here they are stored 0-based
//      – indexable bytes / 4 – accessed by (anObject byteAt: index); Smalltalk
//             indices are 1-based, here they are stored 0-based; four bytes
//             are stored in one field/integer/OP
// 


// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

OP fetchFieldOf(OP op, int fieldIndex) {
	return *(objectFor(op) + fieldIndex);
}

void storeFieldOf(OP op, int fieldIndex, OP valueOp) {
	*(objectFor(op) + fieldIndex) = valueOp;
}

OP fetchObjectFieldOf(OBJ object, int fieldIndex) {
	return *(object + fieldIndex);
}

void storeObjectFieldOf(OBJ object, int fieldIndex, OP valueOp) {
	*(object + fieldIndex) = valueOp;
}

OP fetchNamedPointer(OP op, int fieldIndex) {
	return fetchFieldOf(op, fetchFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD) + fieldIndex);
}

void storeNamedPointer(OP op, int fieldIndex, OP valueOp) {
	// write barrier – storing a pointer to a new object in an old object
	if(isNewObject(valueOp) && !isNewObject(op)) {
		addToRemembered(op);
	}
	
	storeFieldOf(op, fetchFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD) + fieldIndex, valueOp);
}

OP fetchIndexablePointer(OP op, int fieldIndex) {
	return fetchFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + fieldIndex);
}

void storeIndexablePointer(OP op, int fieldIndex, OP valueOp) {
	// write barrier – storing a pointer to a new object in an old object
	if(isNewObject(valueOp) && !isNewObject(op)) {
		addToRemembered(op);
	}
	
	storeFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + fieldIndex, valueOp);
}

#endif

int fetchIndexableByte(OP op, int fieldIndex) {
	OBJ object = objectFor(op);
	unsigned char * bytes = (unsigned char *) (object + *(object + OBJECT_INDEXABLE_BYTE_FIRST_FIELD));
	
	return (int) (*(bytes + fieldIndex));
}

void storeIndexableByte(OP op, int fieldIndex, int value) {
	OBJ object = objectFor(op);
	unsigned char * bytes = (unsigned char *) (object + *(object + OBJECT_INDEXABLE_BYTE_FIRST_FIELD));
	
	*(bytes + fieldIndex) = (unsigned char) value;
}

// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

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

#endif

int fetchIndexableByteLength(OP op) {
	if(isSmallIntegerObject(op)) {
		error("Cannot be a small integer.");
	}
	
	return fetchFieldOf(op, OBJECT_INDEXABLE_BYTE_SIZE_FIELD);
}

// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

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

#endif

OP instantiateClassInSpace(OP class, int indexablePointerSize, int indexableByteSize, int inOldSpace) {
	int namedPointerSize = fetchNamedPointer(class, CLASSDESCRIPTION_TOTAL_INSTANCE_VARIABLE_SIZE);
	
	int fieldSize = namedPointerSize
	              + indexablePointerSize
	              + (indexableByteSize + 3) / 4; // storing 4 bytes in one integer => round to nearest 4 bytes
	
	int size = OBJECT_HEADER_LENGTH + fieldSize;
	
	// when creating new classes (and metaclasses), allocate them directly in the old space
	// since they will probably live forever and (more importantly) the garbage collector
	// does not walk through or update class pointers of objects => classes cannot be moved;
	// symbols and compiled methods will also probably live forever;
	// also, if the object would be too big for the new space (more than half of its size),
	// allocate it directly in the old space
	OP metaclass = fetchClass(class);
	if(!inOldSpace && (class == SYMBOL_OP ||
	                 class == COMPILEDMETHOD_OP ||
	                 metaclass == METACLASS_OP ||
	                 metaclass == METACLASS_CLASS_OP ||
	                 size >= DEFAULT_YOUNG_SPACE_SIZE / 2)) {
		inOldSpace = 1;
	}
	
	OBJ object;
	
	if(inOldSpace) {
		object = allocateOldObjectOfSize(size);
	}
	else {
		object = allocateNewObjectOfSize(size);
	}
	
	OP op = objectPointerFor(object);
	
	storeObjectFieldOf(object, OBJECT_SIZE_FIELD, size);
	storeObjectFieldOf(object, OBJECT_OP_FIELD, op);
	storeObjectFieldOf(object, OBJECT_CLASS_FIELD, class);
	storeObjectFieldOf(object, OBJECT_NAMED_POINTER_FIRST_FIELD, OBJECT_HEADER_LENGTH);
	storeObjectFieldOf(object, OBJECT_NAMED_POINTER_SIZE_FIELD, namedPointerSize);
	storeObjectFieldOf(object, OBJECT_INDEXABLE_POINTER_FIRST_FIELD, OBJECT_HEADER_LENGTH + namedPointerSize);
	storeObjectFieldOf(object, OBJECT_INDEXABLE_POINTER_SIZE_FIELD, indexablePointerSize);
	storeObjectFieldOf(object, OBJECT_INDEXABLE_BYTE_FIRST_FIELD, OBJECT_HEADER_LENGTH + namedPointerSize + indexablePointerSize);
	storeObjectFieldOf(object, OBJECT_INDEXABLE_BYTE_SIZE_FIELD, indexableByteSize);
	
	if(inOldSpace) {
		storeNextRemembered(op, NOT_REMEMBERED);
	}
	else {
		storeObjectFieldOf(object, OBJECT_AGE_FIELD, 0);
	}
	
	// fill memory to zero because there might be remainders of a previously garbage-collected object;
	// pointers will point to nil (op == 0), bytes will be zero
	memset(object + OBJECT_HEADER_LENGTH, 0, sizeof(OP) * fieldSize);
	
	return op;
}
