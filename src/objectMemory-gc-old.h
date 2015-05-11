#ifndef __OBJECTMEMORY_GC_OLD_H__
#define __OBJECTMEMORY_GC_OLD_H__

void garbageCollectOldSpace();
void markPhase();
void allocateMarkTable();
void deallocateMarkTable();
void markAndProcessObjectPointer(OP);
void markAndProcessObject(OBJ);
int isObjectMarked(OBJ);
void markOldObject(OBJ);
void markNewObject(OBJ);
int isOldObjectMarked(OBJ);
int isNewObjectMarked(OBJ);
void compactPhase(OBJ, OBJ);
void moveChunk(OBJ, int, OBJ);
void allocateBreakTable();
void deallocateBreakTable();
void enlargeBreakTable();
void updateOldObjectsPhase(OBJ, OBJ);
void updateYoungObjectsPhase(OBJ, OBJ);
void updateRegistersPhase();
void updateOldObject(OBJ);
void updateYoungObject(OBJ);
void updatePointer(OBJ);
int differenceForPointer(OP);

#endif
