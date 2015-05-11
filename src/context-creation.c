#include <string.h> /* memcpy */
#include "common.h"
#include "constants.h"
#include "context-creation.h"
#include "context-helpers.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


OP createContextForMethod(OP method, OP receiver, OP senderContext, OP selector) {
	OP contextTemplate = fetchNamedPointer(method, COMPILEDMETHOD_CONTEXT_TEMPLATE);
	
	// create a template for the context and save it to the compiled method
	// so that we don't have to create it every time – we just copy it and modify
	// the parameters that are different
	if(contextTemplate == OBJECT_NIL_OP) {
		int parameterCount = fetchNamedSmallInteger(method, COMPILEDMETHOD_PARAMETER_COUNT);
		int temporaryCount = fetchNamedSmallInteger(method, COMPILEDMETHOD_TEMPORARY_COUNT);
		
		int stackSize = fetchNamedSmallInteger(method, COMPILEDMETHOD_STACK_SIZE) + 1;
		// +1 is due to the possibility of sending #doesNotUnderstand: with one argument
		// instead of a unary message (with no arguments) while the stack is full
		
		int indexablePointers = parameterCount + temporaryCount + stackSize;
		
		contextTemplate = instantiateClass(METHODCONTEXT_OP, indexablePointers, 0);
		
		storeNamedPointer(contextTemplate, METHODCONTEXT_METHOD, method);
		storeNamedPointer(contextTemplate, METHODCONTEXT_SELECTOR, selector);
		
		storeNamedSmallInteger(contextTemplate, METHODCONTEXT_PROGRAM_COUNTER, 0);
		storeNamedSmallInteger(contextTemplate, METHODCONTEXT_STACK_POINTER, parameterCount + temporaryCount);
		
		storeNamedPointer(method, COMPILEDMETHOD_CONTEXT_TEMPLATE, contextTemplate);
	}
	
	int size = fetchSize(contextTemplate);
	OBJ contextObj = allocateNewObjectOfSize(size);
	OP context = objectPointerFor(contextObj);
	
	// copy the template
	OBJ contextTemplateObj = objectFor(contextTemplate);
	memcpy(contextObj, contextTemplateObj, sizeof(*contextObj) * size);
	
	// change the header fields (OP and age)
	storeObjectFieldOf(contextObj, OBJECT_OP_FIELD, context);
	storeObjectFieldOf(contextObj, OBJECT_AGE_FIELD, 0);
	
	// receiver, sender context and outer context are different every time
	storeNamedPointer(context, METHODCONTEXT_RECEIVER, receiver);
	storeNamedPointer(context, METHODCONTEXT_SENDER, senderContext);
	storeNamedPointer(context, METHODCONTEXT_OUTER_CONTEXT, context);
	
	return context;
}

OP createContextForClosure(OP closure, OP senderContext) {
	OP contextTemplate = fetchNamedPointer(closure, BLOCKCLOSURE_CONTEXT_TEMPLATE);
	
	// create a template for the context and save it to the closure
	// so that we don't have to create it every time – we just copy it and modify
	// the parameters that are different
	if(contextTemplate == OBJECT_NIL_OP) {
		int stackSize = fetchNamedSmallInteger(closure, BLOCKCLOSURE_STACK_SIZE) + 1;
		// +1 is due to the possibility of sending #doesNotUnderstand: with one argument
		// instead of a unary message (with no arguments) while the stack is full
		
		contextTemplate = instantiateClass(METHODCONTEXT_OP, stackSize, 0);
		
		OP outerContext = fetchNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT);
		OP method = fetchNamedPointer(outerContext, METHODCONTEXT_METHOD);
		OP receiver = fetchNamedPointer(outerContext, METHODCONTEXT_RECEIVER);
		
		storeNamedPointer(contextTemplate, METHODCONTEXT_CLOSURE, closure);
		storeNamedPointer(contextTemplate, METHODCONTEXT_OUTER_CONTEXT, outerContext);
		
		storeNamedPointer(contextTemplate, METHODCONTEXT_RECEIVER, receiver);
		storeNamedPointer(contextTemplate, METHODCONTEXT_METHOD, method);
		
		int initialPc = fetchNamedSmallInteger(closure, BLOCKCLOSURE_INITIAL_PROGRAM_COUNTER);
		storeNamedSmallInteger(contextTemplate, METHODCONTEXT_PROGRAM_COUNTER, initialPc);
		storeNamedSmallInteger(contextTemplate, METHODCONTEXT_STACK_POINTER, 0);
		
		storeNamedPointer(closure, BLOCKCLOSURE_CONTEXT_TEMPLATE, contextTemplate);
	}
	
	int size = fetchSize(contextTemplate);
	OBJ contextObj = allocateNewObjectOfSize(size);
	OP context = objectPointerFor(contextObj);
	
	// copy the template
	OBJ contextTemplateObj = objectFor(contextTemplate);
	memcpy(contextObj, contextTemplateObj, sizeof(*contextObj) * size);
	
	// change the header fields (OP and age)
	storeObjectFieldOf(contextObj, OBJECT_OP_FIELD, context);
	storeObjectFieldOf(contextObj, OBJECT_AGE_FIELD, 0);
	
	// sender context is different every time
	storeNamedPointer(context, METHODCONTEXT_SENDER, senderContext);
	
	// reset all temporary variables to nil, they might be "dirty" from previous block evaluations
	int temporaryCount = fetchNamedSmallInteger(closure, BLOCKCLOSURE_TEMPORARY_COUNT);
	if(temporaryCount > 0) {
		OP outerContext = fetchNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT);
		int parameterCount = fetchNamedSmallInteger(closure, BLOCKCLOSURE_PARAMETER_COUNT);
		int variableOffset = fetchNamedSmallInteger(closure, BLOCKCLOSURE_VARIABLE_OFFSET);
		
		for(int index = 0; index < temporaryCount; ++index) {
			storeIndexablePointer(outerContext, variableOffset + parameterCount + index, OBJECT_NIL_OP);
		}
	}
	
	return context;
}

// reset the closure context to its initial state; it is used when retrying exception handler closures
void resetClosureContext(OP context) {
	OP closure = fetchContextClosure(context);
	OP outerContext = fetchClosureOuterContext(closure);
	
	int initialPc = fetchNamedSmallInteger(closure, BLOCKCLOSURE_INITIAL_PROGRAM_COUNTER);
	storeNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, initialPc);
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

// create the topmost context used by the VM
OP createMainContext() {
	OP context = instantiateClass(METHODCONTEXT_OP, 5, 0);
	
	// the special value INTERPRETER_EOF as the program counter makes the interpreter
	// stop after returning to this context
	storeNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, INTERPRETER_EOF);
	storeNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER, 0);
	
	return context;
}
