#include "common.h"
#include "constants.h"
#include "context-stack.h"
#include "context-stack-active.h"
#include "interpreter.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


void stackPush(OP context, OP value) {
	if(context == getActiveContext()) {
		activeContextStackPush(value);
		return;
	}
	
	int sp = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	OP newValueOp = smallIntegerObjectOf(sp + 1);
	storeNamedPointer(context, METHODCONTEXT_STACK_POINTER, newValueOp);
	
	storeIndexablePointer(context, sp, value);
}

void stackDuplicateTop(OP context) {
	if(context == getActiveContext()) {
		activeContextStackDuplicateTop();
		return;
	}
	
	int sp = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	OP newValueOp = smallIntegerObjectOf(sp + 1);
	storeNamedPointer(context, METHODCONTEXT_STACK_POINTER, newValueOp);
	
	OP stackTop = fetchIndexablePointer(context, sp - 1);
	storeIndexablePointer(context, sp, stackTop);
}

OP stackPop(OP context) {
	if(context == getActiveContext()) {
		return activeContextStackPop();
	}
	
	int sp = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER) - 1;
	OP newValueOp = smallIntegerObjectOf(sp);
	storeNamedPointer(context, METHODCONTEXT_STACK_POINTER, newValueOp);
	
	return fetchIndexablePointer(context, sp);
}

void stackDecrement(OP context, int indexFromTop) {
	if(context == getActiveContext()) {
		activeContextStackDecrement(indexFromTop);
	}
	
	int sp = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER) - indexFromTop;
	OP newValueOp = smallIntegerObjectOf(sp);
	storeNamedPointer(context, METHODCONTEXT_STACK_POINTER, newValueOp);
}

void stackIncrement(OP context, int indexFromTop) {
	stackDecrement(context, -indexFromTop);
}

// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

OP stackPeek(OP context) {
	return stackPeekDown(context, 0);
}

OP stackPeekDown(OP context, int indexFromTop) {
	int sp = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	return fetchIndexablePointer(context, sp - indexFromTop - 1);
}

#endif
