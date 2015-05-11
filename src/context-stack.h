#ifndef __CONTEXT_STACK_H__
#define __CONTEXT_STACK_H__

void stackPush(OP, OP);
void stackDuplicateTop(OP);
OP stackPop(OP);
OP stackPeek(OP);
OP stackPeekDown(OP, int);
void stackCopy(OP, int, int, OP);

#endif
