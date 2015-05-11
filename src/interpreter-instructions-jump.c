#include "common.h"
#include "constants.h"
#include "interpreter-instructions-jump.h"
#include "interpreter-instructions.h"
#include "interpreter.h"
#include "context-stack-active.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


void interpretJump(OP context) {
	int highByte = fetchNextBytecode();
	incrementProgramCounter();
	int lowByte = fetchNextBytecode();
	incrementProgramCounter();
	
	jump((highByte - 128) * 256 + lowByte);
}

void interpretPopJumpIfTrue(OP context) {
	int highByte = fetchNextBytecode();
	incrementProgramCounter();
	int lowByte = fetchNextBytecode();
	incrementProgramCounter();
	OP condition = activeContextStackPop();
	
	if(condition == OBJECT_TRUE_OP) {
		jump((highByte - 128) * 256 + lowByte);
	}
	else if(condition != OBJECT_FALSE_OP) {
		error("PopJumpIfTrue: boolean expected");
	}
}

void interpretPopJumpIfFalse(OP context) {
	int highByte = fetchNextBytecode();
	incrementProgramCounter();
	int lowByte = fetchNextBytecode();
	incrementProgramCounter();
	OP condition = activeContextStackPop();
	
	if(condition == OBJECT_FALSE_OP) {
		jump((highByte - 128) * 256 + lowByte);
	}
	else if(condition != OBJECT_TRUE_OP) {
		error("PopJumpIfFalse: boolean expected");
	}
}

void interpretPopJumpIfNil(OP context) {
	int highByte = fetchNextBytecode();
	incrementProgramCounter();
	int lowByte = fetchNextBytecode();
	incrementProgramCounter();
	OP op = activeContextStackPop();
	
	if(op == OBJECT_NIL_OP) {
		jump((highByte - 128) * 256 + lowByte);
	}
}

void interpretPopJumpIfNotNil(OP context) {
	int highByte = fetchNextBytecode();
	incrementProgramCounter();
	int lowByte = fetchNextBytecode();
	incrementProgramCounter();
	OP op = activeContextStackPop();
	
	if(op != OBJECT_NIL_OP) {
		jump((highByte - 128) * 256 + lowByte);
	}
}
