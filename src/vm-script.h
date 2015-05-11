#ifndef __VM_SCRIPT_H__
#define __VM_SCRIPT_H__

#include <stdio.h>

void processScript();
OP compileScript();
void runScript();
void printResult();
OP fetchScript();
OP loadScriptFile(char *);
OP loadScript(unsigned char *);
void processVmScriptRegisters();
void updateVmScriptRegisters();
void markAndProcessVmScriptRegisters();

#endif
