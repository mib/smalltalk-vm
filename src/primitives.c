#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "constants.h"
#include "primitives.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"
#include "context-creation.h"
#include "context-stack.h"
#include "vm.h"


OP (* primitiveMethods[128])(OP, OP *) = {
	// 0
	NULL,
	primitiveSmallInteger_Add_,
	primitiveSmallInteger_Subtract_,
	primitiveSmallInteger_LessThan_,
	primitiveSmallInteger_GreaterThan_,
	// 5
	primitiveSmallInteger_LessThanOrEqual_,
	primitiveSmallInteger_GreaterThanOrEqual_,
	primitiveSmallInteger_Equal_,
	primitiveSmallInteger_NotEqual_,
	primitiveSmallInteger_Times_,
	// 10
	NULL,
	primitiveSmallInteger_Modulo_,
	primitiveSmallInteger_IntegerDivide_,
	NULL,
	NULL,
	// 15
	NULL, NULL, NULL, NULL, NULL,
	// 20
	NULL, NULL, NULL, NULL, NULL,
	// 25
	NULL, NULL, NULL, NULL, NULL,
	// 30
	NULL, NULL, NULL, NULL, NULL,
	// 35
	NULL, NULL, NULL, NULL, NULL,
	// 40
	NULL, NULL, NULL, NULL, NULL,
	// 45
	NULL, NULL, NULL, NULL, NULL,
	// 50
	NULL, NULL, NULL, NULL, NULL,
	// 55
	NULL, NULL, NULL, NULL, NULL,
	// 60
	primitiveObject_At_,
	primitiveObject_At_Put_,
	primitiveObject_Size,
	NULL,
	NULL,
	// 65
	NULL, NULL, NULL, NULL, NULL,
	// 70
	primitiveBehavior_BasicNew,
	primitiveBehavior_BasicNew_,
	NULL,
	NULL,
	NULL,
	// 75
	NULL, NULL, NULL, NULL, NULL,
	// 80
	NULL, NULL, NULL, NULL, NULL,
	// 85
	NULL, NULL, NULL, NULL, NULL,
	// 90
	NULL, NULL, NULL, NULL, NULL,
	// 95
	NULL, NULL, NULL, NULL, NULL,
	// 100
	NULL, NULL, NULL, NULL, NULL,
	// 105
	NULL,
	primitiveBehavior_BasicNew_Bytes_, // non-standard
	primitiveObject_ByteAt_, // non-standard
	primitiveObject_ByteAt_Put_, // non-standard
	primitiveObject_ByteSize, // non-standard
	// 110
	primitiveProtoObject_Identical_,
	primitiveProtoObject_Class,
	NULL,
	NULL,
	NULL,
	// 115
	NULL,
	NULL,
	primitiveSystemDictionary_At_, // non-standard
	primitiveSystemDictionary_At_Put_, // non-standard
	primitiveSystemDictionary_Size, // non-standard
	// 120
	primitiveBlockClosure_Value, // non-standard
	primitiveBlockClosure_Value_, // non-standard
	primitiveBlockClosure_Value_Value_, // non-standard
	primitiveTranscriptClass_NextPut_, // non-standard
	NULL,
	// 125
	NULL,
	primitiveContext_Retry, // non-standard
	primitiveContext_Resume_, // non-standard
};


// ProtoObject>>#==
OP primitiveProtoObject_Identical_(OP receiver, OP * arguments) {
	return receiver == arguments[0] ? OBJECT_TRUE_OP : OBJECT_FALSE_OP;
}

// ProtoObject>>#class
OP primitiveProtoObject_Class(OP receiver, OP * arguments) {
	return fetchClass(receiver);
}

// Behavior>>#basicNew
OP primitiveBehavior_BasicNew(OP receiver, OP * arguments) {
	return instantiateClass(receiver, 0, 0);
}

// Behavior>>#basicNew:
OP primitiveBehavior_BasicNew_(OP receiver, OP * arguments) {
	return instantiateClass(receiver, smallIntegerValueOf(arguments[0]), 0);
}

// Behavior>>#basicNew:bytes:
OP primitiveBehavior_BasicNew_Bytes_(OP receiver, OP * arguments) {
	return instantiateClass(receiver, smallIntegerValueOf(arguments[0]), smallIntegerValueOf(arguments[1]));
}

// Object>>#at:
OP primitiveObject_At_(OP receiver, OP * arguments) {
	int index = smallIntegerValueOf(arguments[0]) - 1;
	
	return fetchIndexablePointer(receiver, index);
}

// Object>>#at:put:
OP primitiveObject_At_Put_(OP receiver, OP * arguments) {
	int index = smallIntegerValueOf(arguments[0]) - 1;
	OP value = arguments[1];
	
	storeIndexablePointer(receiver, index, value);
	
	return arguments[1];
}

// Object>>#size
OP primitiveObject_Size(OP receiver, OP * arguments) {
	int indexablePointerLength = fetchIndexablePointerLength(receiver);
	
	if(indexablePointerLength) {
		return smallIntegerObjectOf(indexablePointerLength);
	}
	
	return smallIntegerObjectOf(fetchIndexableByteLength(receiver));
}

// Object>>#byteAt:
OP primitiveObject_ByteAt_(OP receiver, OP * arguments) {
	int index = smallIntegerValueOf(arguments[0]) - 1;
	
	return smallIntegerObjectOf(fetchIndexableByte(receiver, index));
}

// Object>>#byteAt:put:
OP primitiveObject_ByteAt_Put_(OP receiver, OP * arguments) {
	int index = smallIntegerValueOf(arguments[0]) - 1;
	int value = smallIntegerValueOf(arguments[1]);
	
	storeIndexableByte(receiver, index, value);
	
	return arguments[1];
}

// Object>>#byteSize
OP primitiveObject_ByteSize(OP receiver, OP * arguments) {
	return smallIntegerObjectOf(fetchIndexableByteLength(receiver));
}

// SmallInteger>>#+
OP primitiveSmallInteger_Add_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first + second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#-
OP primitiveSmallInteger_Subtract_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first - second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#*
OP primitiveSmallInteger_Times_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first * second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#//
OP primitiveSmallInteger_IntegerDivide_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first / second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#\\ (double backslash)
OP primitiveSmallInteger_Modulo_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first % second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#=
OP primitiveSmallInteger_Equal_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return receiver == arguments[0] ? OBJECT_TRUE_OP : OBJECT_FALSE_OP;
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#~=
OP primitiveSmallInteger_NotEqual_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return receiver != arguments[0] ? OBJECT_TRUE_OP : OBJECT_FALSE_OP;
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#<
OP primitiveSmallInteger_LessThan_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) < smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#<=
OP primitiveSmallInteger_LessThanOrEqual_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) <= smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#>
OP primitiveSmallInteger_GreaterThan_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) > smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#>=
OP primitiveSmallInteger_GreaterThanOrEqual_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) >= smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	return PRIMITIVE_FAIL_OP;
}

// Context>>#resume:
OP primitiveContext_Resume_(OP receiver, OP * arguments) {
	stackPush(receiver, arguments[0]);
	switchActiveContext(receiver);
	return receiver;
}

// Context>>#retry
OP primitiveContext_Retry(OP receiver, OP * arguments) {
	// exception handler is tied to a block closure so the context has to be a closure context
	initializeClosureContext(receiver);
	switchActiveContext(receiver);
	return receiver;
}

// BlockClosure>>#value
OP primitiveBlockClosure_Value(OP receiver, OP * arguments) {
	return helperPrimitiveBlockClosure_ValueWithArguments(receiver, arguments, 0);
}

// BlockClosure>>#value:
OP primitiveBlockClosure_Value_(OP receiver, OP * arguments) {
	return helperPrimitiveBlockClosure_ValueWithArguments(receiver, arguments, 1);
}

// BlockClosure>>#value:value:
OP primitiveBlockClosure_Value_Value_(OP receiver, OP * arguments) {
	return helperPrimitiveBlockClosure_ValueWithArguments(receiver, arguments, 2);
}

// helper for BlockClosure>>#value, BlockClosure>>#value: and BlockClosure>>#value:value:
OP helperPrimitiveBlockClosure_ValueWithArguments(OP receiver, OP * arguments, int argumentCount) {
	OP newContext = createContextForClosure(receiver, getActiveContext());
	
	if(argumentCount > 0) {
		int parameterOffset = fetchNamedSmallInteger(receiver, BLOCKCLOSURE_VARIABLE_OFFSET);
		OP outerContext = fetchNamedPointer(receiver, BLOCKCLOSURE_OUTER_CONTEXT);
		
		for(int index = 0; index < argumentCount; ++index) {
			storeIndexablePointer(outerContext, parameterOffset + index, arguments[index]);
		}
		
	}
	
	switchActiveContext(newContext);
	
	// do not return any value now, it will be later returned by the new context
	return PRIMITIVE_VOID_OP;
}

// Transcript class>>#nextPut:
OP primitiveTranscriptClass_NextPut_(OP receiver, OP * arguments) {
	putchar(characterValueOf(arguments[0]));
	
	return arguments[0];
}

// SystemDictionary>>#at:
OP primitiveSystemDictionary_At_(OP receiver, OP * arguments) {
	OP keyArray = fetchNamedPointer(receiver, DICTIONARY_KEY_ARRAY);
	OP keyArraySize = fetchIndexablePointerLength(keyArray);
	
	for(int index = 0; index < keyArraySize; ++index) {
		if(fetchIndexablePointer(keyArray, index) == arguments[0]) {
			OP valueArray = fetchNamedPointer(receiver, DICTIONARY_VALUE_ARRAY);
			return fetchIndexablePointer(valueArray, index);
		}
	}
	
	return OBJECT_NIL_OP;
}

// SystemDictionary>>#at:put:
OP primitiveSystemDictionary_At_Put_(OP receiver, OP * arguments) {
	OP keyArray = fetchNamedPointer(receiver, DICTIONARY_KEY_ARRAY);
	OP keyArraySize = fetchIndexablePointerLength(keyArray);
	
	for(int index = 0; index < keyArraySize; ++index) {
		if(fetchIndexablePointer(keyArray, index) == OBJECT_NIL_OP) {
			storeIndexablePointer(keyArray, index, arguments[0]);
			OP valueArray = fetchNamedPointer(receiver, DICTIONARY_VALUE_ARRAY);
			storeIndexablePointer(valueArray, index, arguments[1]);
			return arguments[1];
		}
	}
	
	// no space available
	return PRIMITIVE_FAIL_OP;
}

// SystemDictionary>>#size
OP primitiveSystemDictionary_Size(OP receiver, OP * arguments) {
	OP keyArray = fetchNamedPointer(receiver, DICTIONARY_KEY_ARRAY);
	OP keyArraySize = fetchIndexablePointerLength(keyArray);
	
	int result = 0;
	for(int index = 0; index < keyArraySize; ++index) {
		result += (fetchIndexablePointer(keyArray, index) != OBJECT_NIL_OP);
	}
	
	return smallIntegerObjectOf(result);
}
