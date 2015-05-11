#include <stdio.h> /* FILE, EOF, fopen, fgetc, ftell, fseek, rewind, fclose, stdin, putchar */
#include <string.h> /* strlen, strcmp */
#include "common.h"
#include "constants.h"
#include "vm-script.h"
#include "vm.h"
#include "context-creation.h"
#include "context-stack.h"
#include "context-stack-active.h"
#include "context-helpers.h"
#include "interpreter.h"
#include "interpreter-sends.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-old.h"
#include "debugTools.h"


// These are all VM registers. They cannot be kept as local variables inside
// functions since a garbage collection might be triggered in the midst
// of running a function. The GC needs to use them as roots of reachability
// and update them if necessary through processVmRegisters().
OP scriptMethod = OBJECT_NIL_OP;  // a compiled version of the script passed as an argument
OP scriptContext = OBJECT_NIL_OP; // context of the executed script
OP scriptResult = OBJECT_NIL_OP;  // return value of the script method

OP mainContext;
OP exceptionHandlerContext;
char * optScriptFile;
unsigned char * optScript;


void processScript() {
	scriptMethod = compileScript();
	
	if(scriptMethod == OBJECT_NIL_OP || fetchClass(scriptMethod) != COMPILEDMETHOD_OP) {
		error("Compilation failed, aborting...\n");
	}
	
	runScript();
}

OP compileScript() {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	switchActiveContext(mainContext);
	
	OP scriptSource = fetchScript();
	
	// ask the compiler to compile the script in context of the UndefinedObject class;
	// send the message #compileSource:forClass: to Compiler
	switchActiveContext(exceptionHandlerContext);
	activeContextStackPush(scriptSource);
	activeContextStackPush(UNDEFINEDOBJECT_OP);
	sendSelector(OBJECT_COMPILE_SOURCE_FOR_CLASS_SELECTOR_OP, COMPILER_OP, 2);
	runInterpreter();
	
	scriptMethod = stackPop(mainContext);
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
	
	return scriptMethod;
}

void runScript() {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	
	// fetch the method and run it in context of the nil object
	scriptContext = createContextForMethod(scriptMethod, OBJECT_NIL_OP, exceptionHandlerContext, OBJECT_NIL_OP);
	switchActiveContext(scriptContext);
	runInterpreter();
	
	scriptResult = stackPop(mainContext);
	
	int shouldPrintResult = 0;
	
	// do not print nil returned from automatically added ^ self at the end of the method
	// or ^ self in the default exception handler when an unhandled exception occurred
	if((
	     fetchNamedSmallInteger(scriptContext, METHODCONTEXT_PROGRAM_COUNTER) < fetchIndexableByteLength(scriptMethod)
	     || fetchIndexableByte(scriptMethod, fetchIndexableByteLength(scriptMethod) - 1) != INSTRUCTION_RETURN_SELF_FROM_METHOD
	   ) && (fetchNamedSmallInteger(fetchContextOuterContext(exceptionHandlerContext), METHODCONTEXT_PROGRAM_COUNTER) < fetchIndexableByteLength(fetchContextMethod(fetchContextOuterContext(exceptionHandlerContext))))) {
		shouldPrintResult = 1;
	}
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
	scriptContext = OBJECT_NIL_OP;
	scriptMethod = OBJECT_NIL_OP;
	
	if(shouldPrintResult) {
		printResult();
	}
	
	scriptResult = OBJECT_NIL_OP;
}

void printResult() {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	
	switchActiveContext(exceptionHandlerContext);
	
	// send a message #printString to the result; it will return the
	// string representation of the receiver
	sendSelector(OBJECT_PRINT_STRING_SELECTOR_OP, scriptResult, 0);
	runInterpreter();
	
	OP resultString = stackPop(mainContext);
	
	printString(resultString);
	putchar('\n');
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
}

OP fetchScript() {
	if(optScript != NULL) {
		return loadScript(optScript);
	}
	if(optScriptFile != NULL) {
		return loadScriptFile(optScriptFile);
	}
	
	// this should never happen, the VM checks if at least one option is present
	return OBJECT_NIL_OP;
}

OP loadScriptFile(char * filename) {
	FILE * scriptFile;
	if(strcmp(filename, "-") == 0) {
		scriptFile = stdin;
	}
	else {
		scriptFile = fopen(filename, "rb");
	}
	
	if(scriptFile == NULL) {
		error("Could not open script file.");
	}
	
	fseek(scriptFile, 0L, SEEK_END);
	long filesize = ftell(scriptFile);
	rewind(scriptFile);
	
	// the compiler expects a method header (selector + parameters) so we add a dummy selector
	unsigned char * dummyMethodHeader = "DoIt\n";
	int dummyHeaderLength = strlen(dummyMethodHeader);
	
	OP scriptSource = instantiateClass(STRING_OP, filesize + dummyHeaderLength, 0);
	
	for(int index = 0; index < dummyHeaderLength; ++index) {
		storeIndexablePointer(scriptSource, index, characterObjectOf(dummyMethodHeader[index]));
	}
	
	OP character;
	for(int index = 0; index < filesize; ++index) {
		character = characterObjectOf(fgetc(scriptFile));
		storeIndexablePointer(scriptSource, index + dummyHeaderLength, character);
	}
	
	fclose(scriptFile);
	
	return scriptSource;
}

OP loadScript(unsigned char * script) {
	int scriptLength = strlen(script);
	
	// the compiler expects a method header (selector + parameters) so we add a dummy selector
	unsigned char * dummyMethodHeader = "DoIt\n";
	int dummyHeaderLength = strlen(dummyMethodHeader);
	
	OP scriptSource = instantiateClass(STRING_OP, scriptLength + dummyHeaderLength, 0);
	
	for(int index = 0; index < dummyHeaderLength; ++index) {
		storeIndexablePointer(scriptSource, index, characterObjectOf(dummyMethodHeader[index]));
	}
	
	OP character;
	for(int index = 0; index < scriptLength; ++index) {
		character = characterObjectOf(script[index]);
		storeIndexablePointer(scriptSource, index + dummyHeaderLength, character);
	}
	
	return scriptSource;
}

// used by new space GC: VM registers act as roots of reachability in garbage collection
void processVmScriptRegisters() {
	if(processNewObject(scriptMethod)) {
		scriptMethod = fetchObjectPointer(scriptMethod);
	}
	if(processNewObject(scriptContext)) {
		scriptContext = fetchObjectPointer(scriptContext);
	}
	if(processNewObject(scriptResult)) {
		scriptResult = fetchObjectPointer(scriptResult);
	}
}

// used by old space GC
void updateVmScriptRegisters() {
	updatePointer(&scriptMethod);
	updatePointer(&scriptContext);
	updatePointer(&scriptResult);
}

// used by old space GC: VM registers act as roots of reachability in garbage collection
void markAndProcessVmScriptRegisters() {
	markAndProcessObjectPointer(scriptMethod);
	markAndProcessObjectPointer(scriptContext);
	markAndProcessObjectPointer(scriptResult);
}
