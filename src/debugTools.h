#ifndef __DEBUGTOOLS_H__
#define __DEBUGTOOLS_H__

void inspect(OP);
void deepInspect(OP, int);
void deepInspectOnLevel(OP, int, int);
int deepInspectNamedPointers(OP, OP, int, int);
void deepInspectIndexablePointers(OP, int, int);
void deepInspectIndexableBytes(OP, int, int);
int stringLength(OP);
void printString(OP);

#endif
