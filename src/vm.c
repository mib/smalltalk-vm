#include <stdio.h> /* FILE, fopen, fgetc, ftell, fseek, rewind, fclose */
#include <string.h> /* strlen */
#include "common.h"
#include "constants.h"
#include "vm.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"
#include "objectMemory-gc.h"
#include "interpreter.h"
#include "interpreter-sends.h"
#include "context-creation.h"
#include "context-stack.h"
#include "context-helpers.h"
#include "debugTools.h"


// These are all VM registers. They cannot be kept as local variables inside
// functions since a garbage collection might be triggered in the midst
// of running a function. The GC needs to use them as roots of reachability
// and update them if necessary through processVmRegisters().
static OP activeContext = OBJECT_NIL_OP;            // currently executed context
static OP mainContext = OBJECT_NIL_OP;              // the top context
static OP exceptionHandlerContext = OBJECT_NIL_OP;  // under the top context, catches unhandled exceptions
static OP scriptMethod = OBJECT_NIL_OP;             // a compiled version of the script passed as an argument
static OP scriptContext = OBJECT_NIL_OP;            // context of the executed script
static OP scriptResult = OBJECT_NIL_OP;             // return value of the script method


int runVm(int argc, char ** argv) {
	if(argc < 2) {
		printf("Usage: %s {IMAGE-FILE} {SCRIPT-FILE}\nExample: %s Smalltalk.image myScript.st\n", argv[0], argv[0]);
		return 1;
	}
	
	allocateAll();
	
	loadImageFile(argv[1]);
	
	scriptMethod = compileScript(argv[2]);
	
	if(scriptMethod == OBJECT_NIL_OP || fetchClass(scriptMethod) != COMPILEDMETHOD_OP) {
		error("Compilation failed, aborting...\n");
	}
	
	passArguments(argc, argv);
	
	runScript();
	
	deallocateAll();
	
	return 0;
}

OP createExceptionHandlerContext() {
	sendSelector(mainContext, OBJECT_DEFAULT_EXCEPTION_HANDLER_CONTEXT_SELECTOR_OP, SCRIPTRUNNER_OP, 0);
	runInterpreter();
	
	exceptionHandlerContext = stackPop(mainContext);
	decrementNamedSmallInteger(exceptionHandlerContext, METHODCONTEXT_PROGRAM_COUNTER);
	
	return exceptionHandlerContext;
}

void runInterpreter() {
	while(nextInstructionCycle(activeContext) == INTERPRETER_CONTINUE);
}

void passArguments(int argc, char ** argv) {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	
	// pass all remaining arguments to the script (the first two are the image filename and the script filename)
	// place them all directly to the old space since they will live forever
	OP argumentArray = instantiateClassInOldSpace(ARRAY_OP, argc - 3, 0);
	
	for(int index = 3; index < argc; ++index) {
		int argLength = strlen(argv[index]);
		
		OP argumentString = instantiateClassInOldSpace(STRING_OP, argLength, 0);
		
		for(int charIndex = 0; charIndex < argLength; ++charIndex) {
			storeIndexablePointer(argumentString, charIndex, characterObjectOf(argv[index][charIndex]));
		}
		
		storeIndexablePointer(argumentArray, index - 3, argumentString);
	}
	
	stackPush(exceptionHandlerContext, argumentArray);
	sendSelector(exceptionHandlerContext, OBJECT_ARGUMENTS_SELECTOR_OP, SCRIPTRUNNER_OP, 1);
	runInterpreter();
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
}

OP compileScript(char * filename) {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	switchActiveContext(mainContext);
	
	OP scriptSource = loadScript(filename);
	
	// ask the compiler to compile the script in context of the UndefinedObject class
	stackPush(exceptionHandlerContext, scriptSource);
	stackPush(exceptionHandlerContext, UNDEFINEDOBJECT_OP);
	sendSelector(exceptionHandlerContext, OBJECT_COMPILE_SOURCE_FOR_CLASS_SELECTOR_OP, COMPILER_OP, 2);
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
	// a hack: do not print nil returned from automatically added ^ self at the end of the method
	// or ^ self in the default exception handler when an unhandled exception occurred
	if((
	     fetchNamedSmallInteger(scriptContext, METHODCONTEXT_PROGRAM_COUNTER) < fetchIndexableByteLength(scriptMethod)
	     || fetchIndexableByte(scriptMethod, fetchIndexableByteLength(scriptMethod) - 2) != 5 /* push self */
	   ) && (fetchNamedSmallInteger(fetchClosureOuterContext(exceptionHandlerContext), METHODCONTEXT_PROGRAM_COUNTER) < fetchIndexableByteLength(fetchContextMethod(fetchClosureOuterContext(exceptionHandlerContext))))) {
		
		shouldPrintResult = 1;
	}
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
	scriptContext = OBJECT_NIL_OP;
	scriptMethod = OBJECT_NIL_OP;
	
	if(shouldPrintResult) {
		printResult();
	}
}

void printResult() {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	
	sendSelector(exceptionHandlerContext, OBJECT_PRINT_STRING_SELECTOR_OP, scriptResult, 0);
	runInterpreter();
	
	OP resultString = stackPop(mainContext);
	
	printString(resultString);
	putchar('\n');
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
}

void loadImageFile(char * filename) {
	FILE * imageFile = fopen(filename, "rb");
	
	if(imageFile == NULL) {
		error("Could not load image file.");
	}
	
	OBJ object;
	OP objectPointer;
	int objectSize;
	int index;
	
	while((objectSize = getImageFileField(imageFile)) != 0) {
		OP objectPointer = getImageFileField(imageFile);
		
		object = allocateOldObjectOfSizeAtPosition(objectSize, objectPointer >> 2);
		
		*object = objectSize;
		*(object + 1) = objectPointer;
		
		for(index = 2; index < objectSize; ++index) {
			*(object + index) = getImageFileField(imageFile);
		}
	}
	
	fclose(imageFile);
}

int getImageFileField(FILE * imageFile) {
	int byte1 = fgetc(imageFile);
	int byte2 = fgetc(imageFile);
	int byte3 = fgetc(imageFile);
	int byte4 = fgetc(imageFile);
	
	if(byte4 == EOF) {
		error("Unexpected end of file encountered when loading image file.");
	}
	
	// in case sizeof(int) > 4 bytes
	if((byte1 & byte2 & byte3 & byte4) == 0xff) {
		return -1;
	}
	
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

OP loadScript(char * filename) {
	FILE * scriptFile = fopen(filename, "rb");
	
	if(scriptFile == NULL) {
		error("Could not open script file.");
	}
	
	fseek(scriptFile, 0L, SEEK_END);
	long filesize = ftell(scriptFile);
	rewind(scriptFile);
	
	// the compiler expects a method header (selector + parameters) so we add a dummy selector
	char * dummyMethodHeader = "DoIt\n";
	int dummyHeaderLength = strlen(dummyMethodHeader);
	
	OP scriptSource = instantiateClass(STRING_OP, filesize + dummyHeaderLength, 0);
	
	for(int index = 0; index < dummyHeaderLength; ++index) {
		storeIndexablePointer(scriptSource, index, characterObjectOf(dummyMethodHeader[index]));
	}
	
	for(int index = 0; index < filesize; ++index) {
		storeIndexablePointer(scriptSource, index + dummyHeaderLength, characterObjectOf(fgetc(scriptFile)));
	}
	
	fclose(scriptFile);
	
	return scriptSource;
}

void processVmRegisters() {
	if(processNewObject(mainContext)) {
		mainContext = fetchObjectPointer(mainContext);
	}
	if(processNewObject(exceptionHandlerContext)) {
		exceptionHandlerContext = fetchObjectPointer(exceptionHandlerContext);
	}
	if(processNewObject(scriptMethod)) {
		scriptMethod = fetchObjectPointer(scriptMethod);
	}
	if(processNewObject(scriptContext)) {
		scriptContext = fetchObjectPointer(scriptContext);
	}
	if(processNewObject(scriptResult)) {
		scriptResult = fetchObjectPointer(scriptResult);
	}
	if(processNewObject(activeContext)) {
		activeContext = fetchObjectPointer(activeContext);
	}
}

void allocateAll() {
	allocateObjectMemory();
}

void deallocateAll() {
	deallocateObjectMemory();
}

void switchActiveContext(OP newContext) {
	activeContext = newContext;
}

OP getActiveContext() {
	return activeContext;
}
