#include "common.h"
#include "constants.h"
#include "interpreter.h"
#include "context-stack.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


void (* bytecodeInstructions[32])(OP);
char * bytecodeInstructionNames[32];


int nextInstructionCycle(OP context) {
	int bytecode = fetchNextBytecode(context);
	
	if(bytecode == INTERPRETER_EOF) {
		return INTERPRETER_EOF;
	}
	
	if(bytecode < 1 || bytecode > 24) {
		printf("Instruction out of range: %d\n", bytecode);
		error("Execution halt.");
	}
	
	(*bytecodeInstructions[bytecode])(context);
	
	return INTERPRETER_CONTINUE;
}

int fetchNextBytecode(OP context) {
	int pc = fetchNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER);
	
	// used by the main context of the VM to stop execution
	if(pc == INTERPRETER_EOF) {
		return INTERPRETER_EOF;
	}
	
	int bytecode = fetchIndexableByte(fetchContextMethod(context), pc);
	
	incrementNamedSmallInteger(context, METHODCONTEXT_PROGRAM_COUNTER);
	
	return bytecode;
}
