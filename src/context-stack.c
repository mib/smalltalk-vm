#include "common.h"
#include "constants.h"
#include "context-stack.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


void stackPush(OP context, OP value) {
	int sp = incrementNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	storeIndexablePointer(context, sp - 1, value);
}

void stackDuplicateTop(OP context) {
	int sp = incrementNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	storeIndexablePointer(context, sp - 1, fetchIndexablePointer(context, sp - 2));
}

OP stackPop(OP context) {
	int sp = decrementNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	return fetchIndexablePointer(context, sp);
}

OP stackPeek(OP context) {
	return stackPeekDown(context, 0);
}

OP stackPeekDown(OP context, int indexFromTop) {
	int sp = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER);
	
	return fetchIndexablePointer(context, sp - indexFromTop - 1);
}

void stackCopy(OP context, int from, int length, OP targetContext) {
	if(from < 0) {
		from = fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER) - from - 1;
	}
	
	for(int index = 0; index < length; ++index) {
		stackPush(targetContext, fetchIndexablePointer(context, from + index));
	}
}
