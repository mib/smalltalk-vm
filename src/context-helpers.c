#include "common.h"
#include "constants.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

OP fetchContextSender(OP context) {
	return fetchNamedPointer(context, METHODCONTEXT_SENDER);
}

OP fetchContextReceiver(OP context) {
	return fetchNamedPointer(context, METHODCONTEXT_RECEIVER);
}

OP fetchContextMethod(OP context) {
	return fetchNamedPointer(context, METHODCONTEXT_METHOD);
}

OP fetchContextClosure(OP context) {
	return fetchNamedPointer(context, METHODCONTEXT_CLOSURE);
}

OP fetchContextOuterContext(OP context) {
	return fetchNamedPointer(context, METHODCONTEXT_OUTER_CONTEXT);
}

OP fetchClosureOuterContext(OP closure) {
	return fetchNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT);
}

OP fetchMethodLiteral(OP method, int index) {
	return fetchIndexablePointer(method, index);
}

OP fetchContextTemporary(OP context, int index) {
	OP outerContext = fetchContextOuterContext(context);
	
	return fetchIndexablePointer(outerContext, index);
}

OP storeContextTemporary(OP context, int index, OP value) {
	OP outerContext = fetchContextOuterContext(context);
	
	storeIndexablePointer(outerContext, index, value);
}

#endif
