#ifndef __OBJECTMEMORY_OBJECTS_H__
#define __OBJECTMEMORY_OBJECTS_H__

OP fetchFieldOf(OP, int);
void storeFieldOf(OP, int, OP);
OP fetchNamedPointer(OP, int);
void storeNamedPointer(OP, int, OP);
OP fetchIndexablePointer(OP, int);
void storeIndexablePointer(OP, int, OP);
int fetchIndexableByte(OP, int);
void storeIndexableByte(OP, int, int);
OP fetchClass(OP);
int fetchNamedPointerLength(OP);
int fetchIndexablePointerLength(OP);
int fetchPointerLength(OP);
int fetchIndexableByteLength(OP);
int fetchInstanceVariableSize(OP);
OP fetchObjectPointer(OP);
int fetchSize(OP);
OP instantiateClass(OP, int, int);
OP instantiateClassInOldSpace(OP, int, int);
OP instantiateClassInSpace(OP, int, int, int);

#endif
