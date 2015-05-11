#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strlen, strcmp */
#include "common.h"
#include "constants.h"
#include "vm.h"
#include "vm-image.h"
#include "vm-script.h"
#include "vm-repl.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-old.h"
#include "interpreter.h"
#include "interpreter-sends.h"
#include "interpreter-sends-lookup.h"
#include "context-creation.h"
#include "context-stack.h"
#include "context-stack-active.h"
#include "primitives-fileStreams.h"


// These are all VM registers. They cannot be kept as local variables inside
// functions since a garbage collection might be triggered in the midst
// of running a function. The GC needs to use them as roots of reachability
// and update them if necessary through processVmRegisters().
OP mainContext = OBJECT_NIL_OP;              // the top context
OP exceptionHandlerContext = OBJECT_NIL_OP;  // under the top context, catches unhandled exceptions


// command-line options
int optPrintStatistics = 0;
int optInteractive = 0;
char * optImageFile = NULL;
char * optScriptFile = NULL;
char * optScript = NULL;

char * programName = NULL;


// memory usage and garbage collection statistics
int gcCount;
double gcPercentMin;
double gcPercentSum;
long gcObjectsMovedToOld;
int gcOldCount;
double gcOldPercentMin;
double gcOldPercentSum;
long totalMemoryAllocated;
long totalObjectsAllocated;


int runVm(int argc, char ** argv) {
	parseArguments(&argc, &argv);
	
	if(optImageFile == NULL) {
		printf("No image file specified.\n\n");
		printUsage();
		return 255;
	}
	
	if(!optInteractive && optScript == NULL && optScriptFile == NULL) {
		printf("No script specified.\n\n");
		printUsage();
		return 255;
	}
	
	allocateAll();
	
	loadImageFile(optImageFile);
	
	passArguments(argc, argv);
	
	if(optInteractive) {
		repl();
	}
	else {
		processScript();
	}
	
	deallocateAll();
	
	if(optPrintStatistics) {
		printf("\x1b[2m");
		printf("\nMemory statistics:\n- objects allocated: %ld\n- size allocated: %.2lf MB\n", totalObjectsAllocated, (double) totalMemoryAllocated / 1000000 * sizeof(OP));
		printf("\nGarbage collection statistics:\n- runs in new space: %d\n  - avg new space reclaimed: %.2f %%\n  - min new space reclaimed: %.2f %%\n  - objects moved to old space: %ld\n- runs in old space: %d\n  - avg old space reclaimed: %.2f %%\n  - min old space reclaimed: %.2f %%\n",
			gcCount,
			gcCount ? (gcPercentSum / gcCount) : 0.0,
			gcCount ? gcPercentMin : 0.0,
			gcObjectsMovedToOld,
			gcOldCount,
			gcOldCount ? (gcOldPercentSum / gcOldCount) : 0.0,
			gcOldCount ? gcOldPercentMin : 0.0);
		printf("\x1b[0m");
	}
	
	return 0;
}

void parseArguments(int * argc, char *** argv) {
	programName = *argv[0];
	--*argc;
	++*argv;
	
	int expectedArgument = IMAGE_FILE;
	int optionsAllowed = 1;
	
	while(*argc > 0) {
		if(optionsAllowed && strcmp((*argv)[0], "--") == 0) {
			optionsAllowed = 0;
		}
		else if(optionsAllowed && (strcmp((*argv)[0], "-s") == 0 || strcmp((*argv)[0], "--stats") == 0)) {
			optPrintStatistics = 1;
		}
		else if(optionsAllowed && (strcmp((*argv)[0], "-i") == 0 || strcmp((*argv)[0], "--repl") == 0)) {
			optInteractive = 1;
		}
		else if(optionsAllowed && (strcmp((*argv)[0], "-r") == 0 || strcmp((*argv)[0], "--run") == 0) && *argc >= 2) {
			optScript = (*argv)[1];
			--*argc;
			++*argv;
			
			if(expectedArgument == SCRIPT_FILE) {
				expectedArgument = NOTHING;
			}
		}
		else if(expectedArgument == IMAGE_FILE) {
			optImageFile = (*argv)[0];
			expectedArgument = optScript == NULL ? SCRIPT_FILE : NOTHING;
		}
		else if(expectedArgument == SCRIPT_FILE) {
			optScriptFile = (*argv)[0];
			expectedArgument = NOTHING;
		}
		else if(expectedArgument == NOTHING) {
			break;
		}
		
		--*argc;
		++*argv;
	}
}

void printUsage() {
	printf("Usage: %s [OPTIONS] {IMAGE-FILE} {SCRIPT-FILE}\n       %s [OPTIONS] {IMAGE-FILE} -r {SCRIPT}\n       %s [OPTIONS] {IMAGE-FILE} -i\n\nExample: %s Smalltalk.image myScript.st\n         %s Smalltalk.image -r 'Transcript show: #hello'\n\n", programName, programName, programName, programName, programName);
	printf("Options:\n");
	printf(" -r {SCRIPT}, --run {SCRIPT}\n                Run a script passed as an argument instead of a script file.\n\n");
	printf(" -i, --repl     Run interactively in a REPL mode.\n\n");
	printf(" -s, --stats    Print memory and garbage collection statistics.\n\n");
}

// create a context that catches unhandled exceptions and prints them
OP createExceptionHandlerContext() {
	switchActiveContext(mainContext);
	sendSelector(OBJECT_DEFAULT_EXCEPTION_HANDLER_CONTEXT_SELECTOR_OP, SCRIPTRUNNER_OP, 0);
	
	runInterpreter();
	
	exceptionHandlerContext = stackPop(mainContext);
	decrementNamedSmallInteger(exceptionHandlerContext, METHODCONTEXT_PROGRAM_COUNTER);
	
	return exceptionHandlerContext;
}

// pass the remaining command-line arguments to the world
void passArguments(int argumentCount, char ** arguments) {
	mainContext = createMainContext();
	exceptionHandlerContext = createExceptionHandlerContext();
	
	// pass all remaining arguments to the script;
	// place them all directly to the old space since they will live forever
	OP argumentArray = instantiateClassInOldSpace(ARRAY_OP, argumentCount, 0);
	
	for(int index = 0; index < argumentCount; ++index) {
		int argLength = strlen(arguments[index]);
		
		OP argumentString = instantiateClassInOldSpace(STRING_OP, argLength, 0);
		
		for(int charIndex = 0; charIndex < argLength; ++charIndex) {
			storeIndexablePointer(argumentString, charIndex, characterObjectOf(arguments[index][charIndex]));
		}
		
		storeIndexablePointer(argumentArray, index, argumentString);
	}
	
	switchActiveContext(exceptionHandlerContext);
	activeContextStackPush(argumentArray);
	sendSelector(OBJECT_ARGUMENTS_SELECTOR_OP, SCRIPTRUNNER_OP, 1);
	runInterpreter();
	
	mainContext = OBJECT_NIL_OP;
	exceptionHandlerContext = OBJECT_NIL_OP;
}

// used by new space GC: VM registers act as roots of reachability in garbage collection
void processVmRegisters() {
	if(processNewObject(mainContext)) {
		mainContext = fetchObjectPointer(mainContext);
	}
	if(processNewObject(exceptionHandlerContext)) {
		exceptionHandlerContext = fetchObjectPointer(exceptionHandlerContext);
	}
	processVmScriptRegisters();
}

// used by old space GC
void updateVmRegisters() {
	updatePointer(&mainContext);
	updatePointer(&exceptionHandlerContext);
	
	updateVmScriptRegisters();
}

// used by old space GC: VM registers act as roots of reachability in garbage collection
void markAndProcessVmRegisters() {
	markAndProcessObjectPointer(mainContext);
	markAndProcessObjectPointer(exceptionHandlerContext);
	
	markAndProcessVmScriptRegisters();
}

void allocateAll() {
	allocateObjectMemory();
	allocateMethodLookupCache();
	allocateFileStreamTable();
}

void deallocateAll() {
	deallocateObjectMemory();
	deallocateMethodLookupCache();
	deallocateFileStreamTable();
}
