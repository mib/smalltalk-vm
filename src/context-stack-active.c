#include "common.h"
#include "constants.h"
#include "context-stack-active.h"
#include "interpreter.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-gc-remembered.h"


// special registers and optimized functions to work with the stack
// of the currently active context


OP activeContext;
OBJ activeContextStack;
int activeContextSp;
int activeContextIsOld;


// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

void activeContextStackSave() {
	OP valueOp = smallIntegerObjectOf(activeContextSp);
	storeNamedPointer(getActiveContext(), METHODCONTEXT_STACK_POINTER, valueOp);
}

void activeContextStackLoad() {
	activeContextSp = fetchNamedSmallInteger(getActiveContext(), METHODCONTEXT_STACK_POINTER);
}

void activeContextStackPush(OP value) {
	*(activeContextStack + activeContextSp++) = value;
	
	// write barrier – storing a pointer to a new object to an old object
	if(activeContextIsOld && isNewObject(value)) {
		addToRemembered(activeContext);
	}
}

OP activeContextStackPop() {
	return *(activeContextStack + --activeContextSp);
}

void activeContextStackDecrement(int indexFromTop) {
	activeContextSp -= indexFromTop;
}

void activeContextStackIncrement(int indexFromTop) {
	activeContextSp += indexFromTop;
}

OP activeContextStackPeek() {
	return activeContextStackPeekDown(0);
}

OP activeContextStackPeekDown(int indexFromTop) {
	return *(activeContextStack + activeContextSp - indexFromTop - 1);
}

#endif

void activeContextStackDuplicateTop() {
	OP stackTop = activeContextStackPeek();
	activeContextStackPush(stackTop);
}
