#ifndef __CONTEXT_CHANGING_H__
#define __CONTEXT_CHANGING_H__

#include <stdio.h>

void returnToContext(OP, OP);
void returnToParentContext(OP);
void returnFromClosure(OP);

#endif
