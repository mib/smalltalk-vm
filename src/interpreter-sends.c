#include "common.h"
#include "constants.h"
#include "interpreter-sends.h"
#include "context-creation.h"
#include "context-helpers.h"
#include "context-stack.h"
#include "vm.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"


OP (* primitiveMethods[128])(OP, OP *);


void sendSelector(OP senderContext, OP selector, OP receiver, int argumentCount) {
	OP lookupClass = fetchClass(receiver);
	
	callMethod(senderContext, receiver, selector, lookupClass, argumentCount);
}

void sendSelectorToSuper(OP senderContext, OP selector, OP receiver, int argumentCount) {
	OP senderMethod = fetchContextMethod(senderContext);
	OP senderMethodClass = fetchNamedPointer(senderMethod, COMPILEDMETHOD_CLASS);
	OP lookupClass = fetchNamedPointer(senderMethodClass, BEHAVIOR_SUPERCLASS);
	
	callMethod(senderContext, receiver, selector, lookupClass, argumentCount);
}

void callMethod(OP senderContext, OP receiver, OP selector, OP lookupClass, int argumentCount) {
	// ensure there is some new space left so that garbage collector will not be triggered
	// during the execution of this method and change OPs stored in local variables
	while(!hasNewSpaceAtLeast(1000)) {
		senderContext = fetchObjectPointer(senderContext);
		receiver = fetchObjectPointer(receiver);
		selector = fetchObjectPointer(selector);
		lookupClass = fetchObjectPointer(lookupClass);
	}
	
	OP method = lookupMethod(selector, lookupClass);
	
	if(method == OBJECT_NIL_OP) {
		if(selector == OBJECT_DOES_NOT_UNDERSTAND_SELECTOR_OP) {
			error("Receiver does not understand #doesNotUnderstand: either.");
		}
		
		sendDoesNotUnderstandMessage(senderContext, receiver, selector, lookupClass, argumentCount);
		return;
	}
	
	OP primitiveOp = fetchNamedPointer(method, COMPILEDMETHOD_PRIMITIVE);
	
	if(primitiveOp != OBJECT_NIL_OP) {
		int primitive = smallIntegerValueOf(primitiveOp);
		OP result = callPrimitive(senderContext, receiver, primitive, argumentCount);
		
		if(result != PRIMITIVE_FAIL_OP) {
			while(argumentCount-- > 0) {
				stackPop(senderContext);
			}
			stackPop(senderContext);
			if(result != PRIMITIVE_VOID_OP) {
				stackPush(senderContext, result);
			}
			return;
		}
	}
	
	OP context = createContextForMethod(method, receiver, senderContext, selector);
	
	for(int index = argumentCount - 1; index >= 0; --index) {
		storeIndexablePointer(context, index, stackPop(senderContext));
	}
	stackPop(senderContext);
	
	switchActiveContext(context);
}

OP callPrimitive(OP senderContext, OP receiver, int primitive, int argumentCount) {
	int stackPointer = fetchNamedSmallInteger(senderContext, METHODCONTEXT_STACK_POINTER);
	
	OP *arguments = objectFor(senderContext) + stackPointer - argumentCount + fetchFieldOf(senderContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	
	return (*primitiveMethods[primitive])(receiver, arguments);
}

OP lookupMethod(OP selector, OP class) {
	if(class == OBJECT_NIL_OP) {
		return OBJECT_NIL_OP;
	}
	
	OP methods = fetchNamedPointer(class, BEHAVIOR_METHODS);
	OP keyArray = fetchNamedPointer(methods, DICTIONARY_KEY_ARRAY);
	int keyCount = fetchIndexablePointerLength(keyArray);
	
	for(int index = 0; index < keyCount; ++index) {
		if(fetchIndexablePointer(keyArray, index) == selector) {
			OP valueArray = fetchNamedPointer(methods, DICTIONARY_VALUE_ARRAY);
			return fetchIndexablePointer(valueArray, index);
		}
	}
	
	OP superclass = fetchNamedPointer(class, BEHAVIOR_SUPERCLASS);
	return lookupMethod(selector, superclass);
}

void sendDoesNotUnderstandMessage(OP senderContext, OP receiver, OP selector, OP lookupClass, int argumentCount) {
	OP arguments = instantiateClass(ARRAY_OP, argumentCount, 0);
	for(int index = argumentCount - 1; index >= 0; --index) {
		storeIndexablePointer(arguments, index, stackPop(senderContext));
	}
	
	OP message = instantiateClass(MESSAGE_OP, 0, 0);
	storeNamedPointer(message, MESSAGE_SELECTOR, selector);
	storeNamedPointer(message, MESSAGE_ARGUMENTS, arguments);
	storeNamedPointer(message, MESSAGE_LOOKUP_CLASS, lookupClass);
	
	stackPush(senderContext, message);
	sendSelector(senderContext, OBJECT_DOES_NOT_UNDERSTAND_SELECTOR_OP, receiver, 1);
}
