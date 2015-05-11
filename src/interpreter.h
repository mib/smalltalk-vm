#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

void runInterpreter();
void switchActiveContext(OP);
void processInterpreterRegisters();
void markAndProcessInterpreterRegisters();
void updateInterpreterRegisters();

#endif
