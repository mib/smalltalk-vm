#define _POSIX_C_SOURCE 200809L /* getline */
#include <stdio.h> /* printf, stdin, getline */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcmp */
#include "common.h"
#include "constants.h"
#include "vm-repl.h"
#include "vm.h"
#include "vm-script.h"
#include "objectMemory-objects.h"


unsigned char * optScript;
OP scriptMethod;


// interactive read-eval-print-loop
void repl() {
	printf("Smalltalk REPL\nTo quit, type 'q', 'quit' or 'exit'.\nThere are no local variables, use globals instead\n(beginning with a capital letter).\n\n");
	
	unsigned char * script = (unsigned char *) malloc(sizeof(*script) * 4101); // "^ " + 4096 + newline (CR+LF) + \0
	
	// add a return to the beginning since there should be only one statement
	// and we want to print its result
	script[0] = '^';
	script[1] = ' ';
	
	char * input = (char *) script + 2;
	size_t inputLength;
	char * inputPosition;
	
	while(1) {
		printf("\x1b[1;31m> \x1b[36m"); // bold red prompt, bold cyan input
		
		if((inputLength = getline(&input, &(size_t) {4096}, stdin)) == -1) {
			break;
		}
		
		printf("\x1b[0m"); // reset everything to default
		
		// remove trailing newlines
		inputPosition = input + inputLength - 1;
		while(inputPosition >= input && (*inputPosition == '\r' || *inputPosition == '\n')) {
			*inputPosition = '\0';
			--inputPosition;
			--inputLength;
		}
		
		if(inputLength == 0) {
			continue;
		}
		
		if(strcmp(input, "q") == 0 || strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
			printf("Bye!\n");
			break;
		}
		
		optScript = script;
		scriptMethod = compileScript();
		
		if(scriptMethod == OBJECT_NIL_OP || fetchClass(scriptMethod) != COMPILEDMETHOD_OP) {
			// the compilation has failed but an error message has already been
			// printed by the default exception handler
			continue;
		}
		
		runScript();
	}
	
	printf("\x1b[0m"); // reset everything to default
	
	free(script);
}
