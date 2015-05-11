#include "common.h"
#include "constants.h"
#include "context-changing.h"
#include "context-stack.h"
#include "context-helpers.h"
#include "vm.h"


void returnToContext(OP context, OP targetContext) {
	OP returnValue = stackPop(context);
	
	stackPush(targetContext, returnValue);
	
	switchActiveContext(targetContext);
}

void returnToParentContext(OP context) {
	OP senderContext = fetchContextSender(context);
	
	returnToContext(context, senderContext);
}

void returnFromClosure(OP context) {
	// returning from a method while inside a block closure: [ ^ true ],
	// the block might be passed on and evaluated in another method/context
	// so we need to locate the context in which it has been created (outerContext)
	// and check if it is still reachable
	
	OP currentContext = context;
	
	OP closure = fetchContextClosure(context);
	OP outerContext = fetchClosureOuterContext(closure);
	
	while(currentContext != outerContext && currentContext != OBJECT_NIL_OP) {
		currentContext = fetchContextSender(currentContext);
	}
	
	if(currentContext != outerContext) {
		// TODO: should raise an exception instead
		error("Cannot return to outer context, it is not reachable any more.");
	}
	
	returnToContext(context, fetchContextSender(outerContext));
}
