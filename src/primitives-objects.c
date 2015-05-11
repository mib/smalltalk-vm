#include <string.h> /* memcpy */
#include "common.h"
#include "constants.h"
#include "primitives-objects.h"
#include "primitives.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "debugTools.h"


// ProtoObject>>#==
OP primitiveProtoObject_Identical_(OP receiver, OP * arguments) {
	return receiver == arguments[0] ? OBJECT_TRUE_OP : OBJECT_FALSE_OP;
}

// ProtoObject>>#class
OP primitiveProtoObject_Class(OP receiver, OP * arguments) {
	return fetchClass(receiver);
}

// Object>>#at:
OP primitiveObject_At_(OP receiver, OP * arguments) {
	// indices are 1-based in Smalltalk but 0-based here
	int index = smallIntegerValueOf(arguments[0]) - 1;
	
	return fetchIndexablePointer(receiver, index);
}

// Object>>#at:put:
OP primitiveObject_At_Put_(OP receiver, OP * arguments) {
	// indices are 1-based in Smalltalk but 0-based here
	int index = smallIntegerValueOf(arguments[0]) - 1;
	OP value = arguments[1];
	
	storeIndexablePointer(receiver, index, value);
	
	return arguments[1];
}

// Object>>#size
OP primitiveObject_Size(OP receiver, OP * arguments) {
	return smallIntegerObjectOf(fetchIndexablePointerLength(receiver));
}

// Object>>#byteAt:
OP primitiveObject_ByteAt_(OP receiver, OP * arguments) {
	// indices are 1-based in Smalltalk but 0-based here
	int index = smallIntegerValueOf(arguments[0]) - 1;
	
	return smallIntegerObjectOf(fetchIndexableByte(receiver, index));
}

// Object>>#byteAt:put:
OP primitiveObject_ByteAt_Put_(OP receiver, OP * arguments) {
	// indices are 1-based in Smalltalk but 0-based here
	int index = smallIntegerValueOf(arguments[0]) - 1;
	int value = smallIntegerValueOf(arguments[1]);
	
	storeIndexableByte(receiver, index, value);
	
	return arguments[1];
}

// Object>>#byteSize
OP primitiveObject_ByteSize(OP receiver, OP * arguments) {
	return smallIntegerObjectOf(fetchIndexableByteLength(receiver));
}

// Object>>#shallowCopy
OP primitiveObject_ShallowCopy(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver)) {
		return receiver;
	}
	
	OP class = fetchClass(receiver);
	
	// characters and symbols are immutable
	if(class == CHARACTER_OP || class == SYMBOL_OP) {
		return receiver;
	}
	
	int size = fetchSize(receiver);
	
	OBJ copyObj = allocateNewObjectOfSize(size);
	
	OP copy = objectPointerFor(copyObj);
	
	OBJ originalObj = objectFor(receiver);
	memcpy(copyObj, originalObj, sizeof(*originalObj) * size);
	
	storeObjectFieldOf(copyObj, OBJECT_OP_FIELD, copy);
	storeObjectFieldOf(copyObj, OBJECT_AGE_FIELD, 0);
	
	return copy;
}

// Object>>#inspectToLevel:
OP primitiveObject_InspectToLevel_(OP receiver, OP * arguments) {
	deepInspect(receiver, smallIntegerValueOf(arguments[0]));
}
