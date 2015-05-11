#include "common.h"
#include "constants.h"
#include "interpreter-instructions-return.h"
#include "interpreter-instructions.h"
#include "interpreter.h"
#include "context-changing.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"


void interpretReturnTopFromMethod(OP context) {
	returnFromMethod();
}

void interpretReturnTopFromBlock(OP context) {
	returnToParentContext();
}

void interpretReturnNilFromMethod(OP context) {
	returnValueFromMethod(OBJECT_NIL_OP);
}

void interpretReturnTrueFromMethod(OP context) {
	returnValueFromMethod(OBJECT_TRUE_OP);
}

void interpretReturnFalseFromMethod(OP context) {
	returnValueFromMethod(OBJECT_FALSE_OP);
}

void interpretReturnSelfFromMethod(OP context) {
	OP receiver = fetchContextReceiver(context);
	returnValueFromMethod(receiver);
}

void interpretReturnInstanceVariable0FromMethod(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 0);
	
	returnValueFromMethod(value);
}

void interpretReturnInstanceVariable1FromMethod(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 1);
	
	returnValueFromMethod(value);
}

void interpretReturnInstanceVariable2FromMethod(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 2);
	
	returnValueFromMethod(value);
}

void interpretReturnTemporaryVariable0FromMethod(OP context) {
	returnValueFromMethod(*(activeContextTemporaries));
}

void interpretReturnTemporaryVariable1FromMethod(OP context) {
	returnValueFromMethod(*(activeContextTemporaries + 1));
}

void interpretReturnTemporaryVariable2FromMethod(OP context) {
	returnValueFromMethod(*(activeContextTemporaries + 2));
}
