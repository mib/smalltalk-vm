#include <stdlib.h>
#include "common.h"
#include "constants.h"
#include "interpreter-instructions.h"
#include "interpreter.h"
#include "interpreter-sends.h"
#include "context-changing.h"
#include "context-stack.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


void interpretPushInstanceVariable(OP context) {
	int index = fetchNextBytecode(context);
	
	stackPush(context, fetchNamedPointer(fetchContextReceiver(context), index));
}

void interpretPushClassVariable(OP context) {
	int index = fetchNextBytecode(context);
	
	stackPush(context, fetchNamedPointer(fetchClass(fetchContextReceiver(context)), index));
}

void interpretPushTemporaryVariable(OP context) {
	int index = fetchNextBytecode(context);
	
	stackPush(context, fetchContextTemporary(context, index));
}

void interpretPushLiteralConstant(OP context) {
	int index = fetchNextBytecode(context);
	
	stackPush(context, fetchContextMethodLiteral(context, index));
}

void interpretPushSelf(OP context) {
	stackPush(context, fetchContextReceiver(context));
}

void interpretPushNil(OP context) {
	stackPush(context, OBJECT_NIL_OP);
}

void interpretPushTrue(OP context) {
	stackPush(context, OBJECT_TRUE_OP);
}

void interpretPushFalse(OP context) {
	stackPush(context, OBJECT_FALSE_OP);
}

void interpretPushSmalltalk(OP context) {
	stackPush(context, OBJECT_SMALLTALK_OP);
}

void interpretPushThisContext(OP context) {
	stackPush(context, context);
}

void interpretStoreIntoInstanceVariable(OP context) {
	int index = fetchNextBytecode(context);
	OP value = stackPeek(context);
	
	storeNamedPointer(fetchContextReceiver(context), index, value);
}

void interpretStoreIntoClassVariable(OP context) {
	int index = fetchNextBytecode(context);
	OP value = stackPeek(context);
	
	storeNamedPointer(fetchClass(fetchContextReceiver(context)), index, value);
}

void interpretStoreIntoTemporaryVariable(OP context) {
	int index = fetchNextBytecode(context);
	OP value = stackPeek(context);
	
	storeContextTemporary(context, index, value);
}

void interpretPop(OP context) {
	stackPop(context);
}

void interpretDuplicateTop(OP context) {
	stackDuplicateTop(context);
}

void interpretReturnTopFromMethod(OP context) {
	OP closure = fetchContextClosure(context);
	
	// returning from a method while inside a closure
	if(closure != OBJECT_NIL_OP) {
		returnFromClosure(context);
	}
	else {
		returnToParentContext(context);
	}
}

void interpretReturnTopFromBlock(OP context) {
	returnToParentContext(context);
}

void interpretSendSelector(OP context) {
	int index = fetchNextBytecode(context);
	int argumentCount = fetchNextBytecode(context);
	OP selector = fetchContextMethodLiteral(context, index);
	OP receiver = stackPeekDown(context, argumentCount);
	
	sendSelector(context, selector, receiver, argumentCount);
}

void interpretSendSelectorToSuper(OP context) {
	int index = fetchNextBytecode(context);
	int argumentCount = fetchNextBytecode(context);
	OP selector = fetchContextMethodLiteral(context, index);
	OP receiver = stackPeekDown(context, argumentCount);
	
	sendSelectorToSuper(context, selector, receiver, argumentCount);
}

void interpretJump(OP context) {
	int highByte = fetchNextBytecode(context);
	int lowByte = fetchNextBytecode(context);
	
	addToNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, (highByte - 128) * 256 + lowByte);
}

void interpretPopJumpIfTrue(OP context) {
	int highByte = fetchNextBytecode(context);
	int lowByte = fetchNextBytecode(context);
	OP condition = stackPop(context);
	
	if(condition == OBJECT_TRUE_OP) {
		addToNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, (highByte - 128) * 256 + lowByte);
	}
	else if(condition != OBJECT_FALSE_OP) {
		error("PopJumpIfTrue: boolean expected");
	}
}

void interpretPopJumpIfFalse(OP context) {
	int highByte = fetchNextBytecode(context);
	int lowByte = fetchNextBytecode(context);
	OP condition = stackPop(context);
	
	if(condition == OBJECT_FALSE_OP) {
		addToNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, (highByte - 128) * 256 + lowByte);
	}
	else if(condition != OBJECT_TRUE_OP) {
		error("PopJumpIfFalse: boolean expected");
	}
}

void interpretPopJumpIfNil(OP context) {
	int highByte = fetchNextBytecode(context);
	int lowByte = fetchNextBytecode(context);
	OP op = stackPop(context);
	
	if(op == OBJECT_NIL_OP) {
		addToNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, (highByte - 128) * 256 + lowByte);
	}
}

void interpretPopJumpIfNotNil(OP context) {
	int highByte = fetchNextBytecode(context);
	int lowByte = fetchNextBytecode(context);
	OP op = stackPop(context);
	
	if(op != OBJECT_NIL_OP) {
		addToNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER, (highByte - 128) * 256 + lowByte);
	}
}


void (* bytecodeInstructions[32])(OP) = {
	// 0
	NULL,
	interpretPushInstanceVariable,
	interpretPushClassVariable,
	interpretPushTemporaryVariable,
	interpretPushLiteralConstant,
	// 5
	interpretPushSelf,
	interpretPushNil,
	interpretPushTrue,
	interpretPushFalse,
	interpretPushSmalltalk,
	// 10
	interpretPushThisContext,
	interpretStoreIntoInstanceVariable,
	interpretStoreIntoClassVariable,
	interpretStoreIntoTemporaryVariable,
	interpretPop,
	// 15
	interpretDuplicateTop,
	interpretReturnTopFromMethod,
	interpretReturnTopFromBlock,
	interpretSendSelector,
	interpretSendSelectorToSuper,
	// 20
	interpretJump,
	interpretPopJumpIfTrue,
	interpretPopJumpIfFalse,
	interpretPopJumpIfNil,
	interpretPopJumpIfNotNil,
};
