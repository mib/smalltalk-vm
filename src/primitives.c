#include "common.h"
#include "constants.h"
#include "primitives.h"
#include "primitives-objects.h"
#include "primitives-arithmetics.h"
#include "primitives-fileStreams.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"
#include "context-creation.h"
#include "context-stack.h"
#include "interpreter.h"
#include "interpreter-sends-lookup.h"
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
	primitiveObject_ShallowCopy,
	NULL,
	NULL,
	NULL,
	NULL,
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
	primitiveFileStream_Open, // non-standard
	primitiveFileStream_Close, // non-standard
	primitiveFileStream_AtEnd, // non-standard
	// 115
	primitiveFileStream_Next, // non-standard
	primitiveFileStream_NextPut_, // non-standard
	primitiveSystemDictionary_At_, // non-standard
	primitiveSystemDictionary_At_Put_, // non-standard
	primitiveSystemDictionary_Size, // non-standard
	// 120
	primitiveBlockClosure_Value, // non-standard
	primitiveBlockClosure_Value_, // non-standard
	primitiveBlockClosure_Value_Value_, // non-standard
	primitiveTranscriptClass_NextPut_, // non-standard
	primitiveObject_InspectToLevel_, // non-standard
	// 125
	primitiveBehavior_AddSelector_WithMethod_, // non-standard
	primitiveContext_Retry, // non-standard
	primitiveContext_Resume_, // non-standard
};


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

// Behavior>>#addSelector:withMethod:
OP primitiveBehavior_AddSelector_WithMethod_(OP receiver, OP * arguments) {
	// this primitive always fails, it is used to update the method lookup cache;
	// the original method itself adds the compiled method to the receiver's
	// method dictionary
	
	removeSelectorFromMethodLookupCache(arguments[0]);
	
	// always fail so that the original method is executed
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
	resetClosureContext(receiver);
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
	
	int nilIndex = -1;
	int index;
	OP key;
	
	for(index = 0; index < keyArraySize; ++index) {
		key = fetchIndexablePointer(keyArray, index);
		if(key == arguments[0]) {
			// the key is already there, overwrite the value
			OP valueArray = fetchNamedPointer(receiver, DICTIONARY_VALUE_ARRAY);
			storeIndexablePointer(valueArray, index, arguments[1]);
			return arguments[1];
		}
		else if(key == OBJECT_NIL_OP && nilIndex == -1) {
			nilIndex = index;
		}
	}
	
	// adding to an empty space
	if(nilIndex != -1) {
		OP valueArray = fetchNamedPointer(receiver, DICTIONARY_VALUE_ARRAY);
		storeIndexablePointer(keyArray, nilIndex, arguments[0]);
		storeIndexablePointer(valueArray, nilIndex, arguments[1]);
		
		return arguments[1];
	}
	
	// no available space => fail this primitive and the run the method which
	// will grow the collection
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
