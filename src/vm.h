#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>

int runVm(int, char **);
OP createExceptionHandlerContext();
void runInterpreter();
void passArguments(int, char **);
OP compileScript(char *);
void runScript();
void printResult();
void loadImageFile(char *);
int getImageFileField(FILE *);
OP loadScript(char *);
void processVmRegisters();
void allocateAll();
void deallocateAll();
void switchActiveContext(OP);
OP getActiveContext();

#endif
