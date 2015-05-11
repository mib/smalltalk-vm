#ifndef __VM_H__
#define __VM_H__

#include <stdio.h>

int runVm(int, char **);
void parseArguments(int *, char ***);
void printUsage();
OP createExceptionHandlerContext();
void passArguments(int, char **);
void processVmRegisters();
void updateVmRegisters();
void markAndProcessVmRegisters();
void allocateAll();
void deallocateAll();

#endif
