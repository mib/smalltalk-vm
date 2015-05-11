#ifndef __OBJECTMEMORY_GC_H__
#define __OBJECTMEMORY_GC_H__

void garbageCollectNewSpace();
void switchNewAndSurvivorSpace();
int processNewObject(OP);
void processAllPointers();
int processObjectPointers(OP);
OP copyObjectToSurvivorSpace(OP);
OP copyObjectToOldSpace(OP);
int fetchObjectAge(OP);
int incrementObjectAge(OP);

#endif
