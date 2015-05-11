#include "common.h"
#include "constants.h"
#include "interpreter-instructions-stack.h"
#include "context-stack-active.h"


void interpretPop(OP context) {
	activeContextStackDecrement(1);
}

void interpretDuplicateTop(OP context) {
	activeContextStackDuplicateTop();
}
