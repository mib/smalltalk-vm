#include "common.h"
#include "constants.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"


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

OP fetchContextTemporary(OP context, int index) {
	OP closure = fetchContextClosure(context);
	
	if(closure != OBJECT_NIL_OP) {
		return fetchIndexablePointer(fetchClosureOuterContext(closure), index);
	}
	
	return fetchIndexablePointer(context, index);
}

OP storeContextTemporary(OP context, int index, OP value) {
	OP closure = fetchContextClosure(context);
	
	if(closure != OBJECT_NIL_OP) {
		storeIndexablePointer(fetchClosureOuterContext(closure), index, value);
	}
	
	storeIndexablePointer(context, index, value);
}

OP fetchContextMethodLiteral(OP context, int index) {
	OP method = fetchContextMethod(context);
	return fetchIndexablePointer(method, index);
}

OP fetchClosureOuterContext(OP closure) {
	return fetchNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT);
}
