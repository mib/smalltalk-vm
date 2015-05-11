#ifndef __OBJECTMEMORY_GC_REMEMBERED_H__
#define __OBJECTMEMORY_GC_REMEMBERED_H__

void addToRemembered(OP);
void checkAndAddToRemembered(OP);
void removeFromRemembered(OP);
OP fetchPrevRemembered(OP);
OP fetchNextRemembered(OP);
void storeNextRemembered(OP, OP);
void processRememberedSet();

#endif
