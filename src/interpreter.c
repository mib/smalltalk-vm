#include "common.h"
#include "constants.h"
#include "interpreter.h"
#include "context-stack-active.h"
#include "context-helpers.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-old.h"


void (* bytecodeInstructions[128])(OP);

// This is a register to store the currently executed context.
OP activeContext = OBJECT_NIL_OP;

// These registers are used to speed up access to parts of the active context
// that are used very often (in all or almost all cycles of the interpreter).
// They are not necessary for the interpreter to function correctly.
int activeContextIsOld;
int activeContextPc;
OP activeContextMethod;
unsigned char * activeContextMethodBytes;
OP activeContextOuterContext;
OBJ activeContextTemporaries;
OBJ activeContextStack;
int activeContextSp;


void runInterpreter() {
	int bytecode;
	
	while(1) {
		// used by the main context of the VM to stop execution
		if(activeContextPc == INTERPRETER_EOF) {
			activeContextStackSave();
			return;
		}
		
		bytecode = fetchNextBytecode();
		incrementProgramCounter();
		
		(*bytecodeInstructions[bytecode])(activeContext);
	}
}

void switchActiveContext(OP newContext) {
	if(activeContext != OBJECT_NIL_OP) {
		// save the information stored in registers to the object itself
		activeContextStackSave();
		storeNamedSmallInteger(activeContext, METHODCONTEXT_PROGRAM_COUNTER, activeContextPc);
	}
	
	activeContext = newContext;
	
	// the following fields are populated to optimize access to often-used places
	
	OBJ activeContextObj = objectFor(activeContext);
	OBJ activeContextObjFields = activeContextObj + *(activeContextObj + OBJECT_NAMED_POINTER_FIRST_FIELD);
	
	activeContextIsOld = isOldObject(activeContext);
	
	activeContextPc = smallIntegerValueOf(*(activeContextObjFields + METHODCONTEXT_PROGRAM_COUNTER));
	activeContextMethod = *(activeContextObjFields + METHODCONTEXT_METHOD);
	activeContextMethodBytes = (unsigned char *) (objectFor(activeContextMethod) + fetchFieldOf(activeContextMethod, OBJECT_INDEXABLE_BYTE_FIRST_FIELD));
	
	activeContextOuterContext = *(activeContextObjFields + METHODCONTEXT_OUTER_CONTEXT);
	activeContextTemporaries = objectFor(activeContextOuterContext) + fetchFieldOf(activeContextOuterContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	activeContextStack = activeContextObj + *(activeContextObj + OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	activeContextSp = smallIntegerValueOf(*(activeContextObjFields + METHODCONTEXT_STACK_POINTER));
}

// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

int fetchNextBytecode() {
	return (int) (*(activeContextMethodBytes + activeContextPc));
}

void incrementProgramCounter() {
	++activeContextPc;
}

OP getActiveContext() {
	return activeContext;
}

void jump(int offset) {
	activeContextPc += offset;
}

#endif

// used by new space GC: interpreter registers act as roots of reachability in garbage collection
void processInterpreterRegisters() {
	if(processNewObject(activeContext)) {
		activeContext = fetchObjectPointer(activeContext);
		activeContextIsOld = isOldObject(activeContext);
		activeContextStack = objectFor(activeContext) + fetchFieldOf(activeContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	}
	if(processNewObject(activeContextOuterContext)) {
		activeContextOuterContext = fetchObjectPointer(activeContextOuterContext);
		activeContextTemporaries = objectFor(activeContextOuterContext) + fetchFieldOf(activeContextOuterContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	}
	if(processNewObject(activeContextMethod)) {
		activeContextMethod = fetchObjectPointer(activeContextMethod);
		activeContextMethodBytes = (unsigned char *) (objectFor(activeContextMethod) + fetchFieldOf(activeContextMethod, OBJECT_INDEXABLE_BYTE_FIRST_FIELD));
	}
}

// used by old space GC: interpreter registers act as roots of reachability in garbage collection
void markAndProcessInterpreterRegisters() {
	markAndProcessObjectPointer(activeContext);
}

// used by old space GC
void updateInterpreterRegisters() {
	updatePointer(&activeContext);
	updatePointer(&activeContextOuterContext);
	updatePointer(&activeContextMethod);
	
	activeContextIsOld = isOldObject(activeContext);
	activeContextTemporaries = objectFor(activeContextOuterContext) + fetchFieldOf(activeContextOuterContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	activeContextStack = objectFor(activeContext) + fetchFieldOf(activeContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
	activeContextMethodBytes = (unsigned char *) (objectFor(activeContextMethod) + fetchFieldOf(activeContextMethod, OBJECT_INDEXABLE_BYTE_FIRST_FIELD));
}
