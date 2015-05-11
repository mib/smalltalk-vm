#ifndef __OBJECTMEMORY_GC_H__
#define __OBJECTMEMORY_GC_H__

#include <stdio.h>

void garbageCollectNewSpace();
void switchNewAndSurvivorSpace();
int processNewObject(OP);
int processObjectPointers(OP);
OP copyObjectToSurvivorSpace(OP);
OP copyObjectToOldSpace(OP);
int fetchObjectAge(OP);
int incrementObjectAge(OP);

#endif
