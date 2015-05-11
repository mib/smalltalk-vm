#include "common.h"
#include "constants.h"
#include "context-changing.h"
#include "context-stack-active.h"
#include "context-helpers.h"
#include "interpreter.h"


void returnToContext(OP targetContext) {
	OP returnValue = activeContextStackPop();
	
	switchActiveContext(targetContext);
	activeContextStackPush(returnValue);
}

// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

// returnValue*() functions are used by optimized instructions combining push + return
void returnValueToContext(OP targetContext, OP returnValue) {
	switchActiveContext(targetContext);
	activeContextStackPush(returnValue);
}

void returnToParentContext() {
	OP senderContext = fetchContextSender(getActiveContext());
	
	returnToContext(senderContext);
}

void returnValueToParentContext(OP returnValue) {
	OP senderContext = fetchContextSender(getActiveContext());
	
	returnValueToContext(senderContext, returnValue);
}

#endif

void returnFromMethod() {
	OP closure = fetchContextClosure(getActiveContext());
	
	// returning from a method while inside a closure
	if(closure != OBJECT_NIL_OP) {
		returnFromClosure();
	}
	else {
		returnToParentContext();
	}
}

void returnValueFromMethod(OP returnValue) {
	OP closure = fetchContextClosure(getActiveContext());
	
	// returning from a method while inside a closure
	if(closure != OBJECT_NIL_OP) {
		returnValueFromClosure(returnValue);
	}
	else {
		returnValueToParentContext(returnValue);
	}
}

void returnFromClosure() {
	// returning from a method while inside a block closure: [ ^ true ],
	// the block might be passed on and evaluated in another method/context
	// so we need to locate the context in which it has been created (outerContext)
	// and check if it is still reachable
	
	OP currentContext = getActiveContext();
	
	OP closure = fetchContextClosure(currentContext);
	OP outerContext = fetchClosureOuterContext(closure);
	
	while(currentContext != outerContext && currentContext != OBJECT_NIL_OP) {
		currentContext = fetchContextSender(currentContext);
	}
	
	if(currentContext != outerContext) {
		// this might raise an exception instead
		error("Cannot return to outer context, it is not reachable any more.");
	}
	
	returnToContext(fetchContextSender(outerContext));
}

void returnValueFromClosure(OP returnValue) {
	// returning from a method while inside a block closure: [ ^ true ],
	// the block might be passed on and evaluated in another method/context
	// so we need to locate the context in which it has been created (outerContext)
	// and check if it is still reachable
	
	OP currentContext = getActiveContext();
	
	OP closure = fetchContextClosure(currentContext);
	OP outerContext = fetchClosureOuterContext(closure);
	
	while(currentContext != outerContext && currentContext != OBJECT_NIL_OP) {
		currentContext = fetchContextSender(currentContext);
	}
	
	if(currentContext != outerContext) {
		// this might raise an exception instead
		error("Cannot return to outer context, it is not reachable any more.");
	}
	
	returnValueToContext(fetchContextSender(outerContext), returnValue);
}
