#include "common.h"
#include "constants.h"
#include "interpreter-instructions-store.h"
#include "interpreter.h"
#include "context-stack-active.h"
#include "context-helpers.h"
#include "objectMemory-objects.h"


void interpretStoreIntoInstanceVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP value = activeContextStackPeek();
	OP receiver = fetchContextReceiver(context);
	
	storeNamedPointer(receiver, index, value);
}

void interpretStoreIntoClassVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP value = activeContextStackPeek();
	OP receiver = fetchContextReceiver(context);
	OP class = fetchClass(receiver);
	
	storeNamedPointer(class, index, value);
}

void interpretStoreIntoTemporaryVariable(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP value = activeContextStackPeek();
	
	storeIndexablePointer(activeContextOuterContext, index, value);
}
