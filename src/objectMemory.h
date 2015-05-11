#ifndef __OBJECTMEMORY_H__
#define __OBJECTMEMORY_H__

void allocateObjectMemory();
void deallocateObjectMemory();
void noSpaceLeftInOldSpace();
OBJ allocateNewObjectOfSize(int);
OBJ allocateSurvivorObjectOfSize(int);
OBJ allocateOldObjectOfSize(int);
OBJ allocateOldObjectOfSizeAtPosition(int, int);

#endif
