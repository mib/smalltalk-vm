#ifndef __CONTEXT_STACK_H__
#define __CONTEXT_STACK_H__

void stackPush(OP, OP);
void stackDuplicateTop(OP);
OP stackPop(OP);
void stackDecrement(OP, int);
void stackIncrement(OP, int);

#endif
