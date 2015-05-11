#ifndef __CONTEXT_CREATION_H__
#define __CONTEXT_CREATION_H__

OP createContextForMethod(OP, OP, OP, OP);
OP createContextForClosure(OP, OP);
void initializeClosureContext(OP);
OP createMainContext();

#endif
