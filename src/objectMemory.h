#ifndef __OBJECTMEMORY_H__
#define __OBJECTMEMORY_H__

void allocateObjectMemory();
void deallocateObjectMemory();
OBJ allocateNewObjectOfSize(int);
int hasNewSpaceAtLeast(int);
OBJ allocateSurvivorObjectOfSize(int);
OBJ allocateOldObjectOfSize(int);
OBJ allocateOldObjectOfSizeAtPosition(int, int);
int isNewObject(OP);
int isSurvivorObject(OP);
int isYoungObject(OP);
int isOldObject(OP);
OBJ objectFor(OP);
OP objectPointerFor(OBJ);

#endif
