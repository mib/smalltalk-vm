#include "common.h"
#include "constants.h"
#include "interpreter-instructions-closure.h"
#include "interpreter-instructions.h"
#include "interpreter.h"
#include "context-stack-active.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"


void interpretCreateBlockClosure(OP context) {
	int parameterCount = fetchNextBytecode();
	incrementProgramCounter();
	int temporaryCount = fetchNextBytecode();
	incrementProgramCounter();
	int variableOffset = fetchNextBytecode();
	incrementProgramCounter();
	OP stackSize = activeContextStackPop();
	OP outerContext = fetchContextOuterContext(getActiveContext());
	int programCounter = activeContextPc + 3;
	
	OP closure = instantiateClass(BLOCKCLOSURE_OP, 0, 0);
	
	storeNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT, outerContext);
	storeNamedSmallInteger(closure, BLOCKCLOSURE_INITIAL_PROGRAM_COUNTER, programCounter);
	storeNamedSmallInteger(closure, BLOCKCLOSURE_PARAMETER_COUNT, parameterCount);
	storeNamedSmallInteger(closure, BLOCKCLOSURE_TEMPORARY_COUNT, temporaryCount);
	storeNamedSmallInteger(closure, BLOCKCLOSURE_VARIABLE_OFFSET, variableOffset);
	storeNamedPointer(closure, BLOCKCLOSURE_STACK_SIZE, stackSize);
	
	activeContextStackPush(closure);
}
