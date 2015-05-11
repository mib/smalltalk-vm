#include "common.h"
#include "constants.h"
#include "context-creation.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


OP createContextForMethod(OP method, OP receiver, OP senderContext, OP selector) {
	int parameterCount = fetchNamedSmallInteger(method, COMPILEDMETHOD_PARAMETER_COUNT);
	int temporaryCount = fetchNamedSmallInteger(method, COMPILEDMETHOD_TEMPORARY_COUNT);
	
	int stackSize = fetchNamedSmallInteger(method, COMPILEDMETHOD_STACK_SIZE) + 1;
	// stackSize + 1 is due to the possibility of sending #doesNotUnderstand: with one argument
	// instead of a unary message with no arguments while the stack is full
	
	int indexablePointers = parameterCount + temporaryCount + stackSize;
	
	OP context = instantiateClass(METHODCONTEXT_OP, indexablePointers, 0);
	
	storeNamedPointer(context, METHODCONTEXT_RECEIVER, receiver);
	storeNamedPointer(context, METHODCONTEXT_SENDER, senderContext);
	storeNamedPointer(context, METHODCONTEXT_METHOD, method);
	storeNamedPointer(context, METHODCONTEXT_OUTER_CONTEXT, context);
	storeNamedPointer(context, METHODCONTEXT_SELECTOR, selector);
	
	storeNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, 0);
	storeNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER, parameterCount + temporaryCount);
	
	return context;
}

OP createContextForClosure(OP closure, OP senderContext) {
	// closure context shares temporary variables (and parameters) with its outer context,
	// therefore we don't allocate any space for them
	
	int stackSize = fetchNamedSmallInteger(closure, BLOCKCLOSURE_STACK_SIZE) + 1;
	// stackSize + 1 is due to the possibility of sending #doesNotUnderstand: with one argument
	// instead of a unary message with no arguments while the stack is full
	
	int indexablePointers = stackSize;
	
	OP outerContext = fetchNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT);
	OP method = fetchNamedPointer(outerContext, METHODCONTEXT_METHOD);
	OP receiver = fetchNamedPointer(outerContext, METHODCONTEXT_RECEIVER);
	
	OP context = instantiateClass(METHODCONTEXT_OP, indexablePointers, 0);
	
	storeNamedPointer(context, METHODCONTEXT_RECEIVER, receiver);
	storeNamedPointer(context, METHODCONTEXT_SENDER, senderContext);
	storeNamedPointer(context, METHODCONTEXT_METHOD, method);
	storeNamedPointer(context, METHODCONTEXT_CLOSURE, closure);
	storeNamedPointer(context, METHODCONTEXT_OUTER_CONTEXT, outerContext);
	
	initializeClosureContext(context);
	
	return context;
}

void initializeClosureContext(OP context) {
	OP closure = fetchContextClosure(context);
	OP outerContext = fetchClosureOuterContext(closure);
	
	storeNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, fetchNamedSmallInteger(closure, BLOCKCLOSURE_INITIAL_PROGRAM_COUNTER));
	storeNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER, 0);
	
	// reset all temporary variables to nil, they might be "dirty" from previous block evaluations
	int temporaryCount = fetchNamedSmallInteger(closure, BLOCKCLOSURE_TEMPORARY_COUNT);
	if(temporaryCount > 0) {
		int parameterCount = fetchNamedSmallInteger(closure, BLOCKCLOSURE_PARAMETER_COUNT);
		int variableOffset = fetchNamedSmallInteger(closure, BLOCKCLOSURE_VARIABLE_OFFSET);
		
		for(int index = 0; index < temporaryCount; ++index) {
			storeIndexablePointer(outerContext, variableOffset + parameterCount + index, OBJECT_NIL_OP);
		}
	}
}

OP createMainContext() {
	OP context = instantiateClass(METHODCONTEXT_OP, 5, 0);
	
	storeNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, INTERPRETER_EOF);
	storeNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER, 0);
	
	return context;
}
