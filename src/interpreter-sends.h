#ifndef __INTERPRETER_SENDS_H__
#define __INTERPRETER_SENDS_H__

void sendSelectorToSuper(OP, OP, int);
void processInterpreterCallRegisters();
void updateInterpreterCallRegisters();
void callMethod(OP, OP, OP, int);
void sendDoesNotUnderstandMessage(OP, OP, OP, int);

#endif
