#include <stdio.h> /* NULL */
#include "common.h"
#include "constants.h"
#include "interpreter-instructions-push.h"
#include "interpreter-instructions.h"
#include "interpreter.h"
#include "context-stack-active.h"
#include "context-helpers.h"
#include "primitives.h"
#include "primitives-objects.h"


void interpretPushInstanceVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, index);
	
	activeContextStackPush(value);
}

void interpretPushClassVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP receiver = fetchContextReceiver(context);
	OP class = fetchClass(receiver);
	OP value = fetchNamedPointer(class, index);
	
	activeContextStackPush(value);
}

void interpretPushTemporaryVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	
	OP value = *(activeContextTemporaries + index);
	
	activeContextStackPush(value);
}

void interpretPushGlobalVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP method = fetchContextMethod(context);
	OP name = fetchMethodLiteral(method, index);
	
	OP arguments[1] = {name};
	OP value = primitiveSystemDictionary_At_(OBJECT_SMALLTALK_OP, arguments);
	
	activeContextStackPush(value);
}

void interpretPushLiteralConstant(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP method = fetchContextMethod(context);
	OP constant = fetchMethodLiteral(method, index);
	OP copy = primitiveObject_ShallowCopy(constant, NULL);
	
	activeContextStackPush(copy);
}

void interpretPushSelf(OP context) {
	OP receiver = fetchContextReceiver(context);
	
	activeContextStackPush(receiver);
}

void interpretPushNil(OP context) {
	activeContextStackPush(OBJECT_NIL_OP);
}

void interpretPushTrue(OP context) {
	activeContextStackPush(OBJECT_TRUE_OP);
}

void interpretPushFalse(OP context) {
	activeContextStackPush(OBJECT_FALSE_OP);
}

void interpretPushSmalltalk(OP context) {
	activeContextStackPush(OBJECT_SMALLTALK_OP);
}

void interpretPushThisContext(OP context) {
	activeContextStackPush(context);
}

void interpretPushInstanceVariable0(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 0);
	
	activeContextStackPush(value);
}

void interpretPushInstanceVariable1(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 1);
	
	activeContextStackPush(value);
}

void interpretPushInstanceVariable2(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 2);
	
	activeContextStackPush(value);
}

void interpretPushInstanceVariable3(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 3);
	
	activeContextStackPush(value);
}

void interpretPushInstanceVariable4(OP context) {
	OP receiver = fetchContextReceiver(context);
	OP value = fetchNamedPointer(receiver, 4);
	
	activeContextStackPush(value);
}

void interpretPushTemporaryVariable0(OP context) {
	activeContextStackPush(*(activeContextTemporaries));
}

void interpretPushTemporaryVariable1(OP context) {
	activeContextStackPush(*(activeContextTemporaries + 1));
}

void interpretPushTemporaryVariable2(OP context) {
	activeContextStackPush(*(activeContextTemporaries + 2));
}

void interpretPushTemporaryVariable3(OP context) {
	activeContextStackPush(*(activeContextTemporaries + 3));
}

void interpretPushTemporaryVariable4(OP context) {
	activeContextStackPush(*(activeContextTemporaries + 4));
}

void interpretPushSmallInteger1(OP context) {
	activeContextStackPush(OBJECT_SMALLINTEGER_1_OP);
}

void interpretPushSmallInteger0(OP context) {
	activeContextStackPush(OBJECT_SMALLINTEGER_0_OP);
}

void interpretPushSmallIntegerNeg1(OP context) {
	activeContextStackPush(OBJECT_SMALLINTEGER_NEG_1_OP);
}
