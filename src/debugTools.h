#ifndef __DEBUGTOOLS_H__
#define __DEBUGTOOLS_H__

#include <stdio.h>

void inspect(OP);
void finspect(FILE *, OP);
void deepInspect(OP, int);
void fdeepInspect(FILE *, OP, int);
void deepInspectOnLevel(OP, int, int);
void fdeepInspectOnLevel(FILE *, OP, int, int);
int fdeepInspectNamedPointers(FILE *, OP, OP, int, int);
void fdeepInspectIndexablePointers(FILE *, OP, int, int);
void fdeepInspectIndexableBytes(FILE *, OP, int, int);
int stringLength(OP);
void printString(OP);
void fprintString(FILE *, OP);

#endif
