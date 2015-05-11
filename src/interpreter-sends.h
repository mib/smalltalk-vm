#ifndef __INTERPRETER_SENDS_H__
#define __INTERPRETER_SENDS_H__

#include <stdio.h>

void sendSelector(OP, OP, OP, int);
void sendSelectorToSuper(OP, OP, OP, int);
void callMethod(OP, OP, OP, OP, int);
OP callPrimitive(OP, OP, int, int);
OP lookupMethod(OP, OP);
void sendDoesNotUnderstandMessage(OP, OP, OP, OP, int);

#endif
