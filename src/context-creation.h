#ifndef __CONTEXT_CREATION_H__
#define __CONTEXT_CREATION_H__

OP createContextForMethod(OP, OP, OP, OP);
OP createContextForClosure(OP, OP);
void resetClosureContext(OP);
OP createMainContext();

#endif
