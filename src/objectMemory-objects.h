#ifndef __OBJECTMEMORY_OBJECTS_H__
#define __OBJECTMEMORY_OBJECTS_H__

int fetchIndexableByte(OP, int);
void storeIndexableByte(OP, int, int);
int fetchIndexableByteLength(OP);
OP instantiateClassInSpace(OP, int, int, int);

#endif
