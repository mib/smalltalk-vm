#ifndef __PRIMITIVES_OBJECTS_H__
#define __PRIMITIVES_OBJECTS_H__

OP primitiveProtoObject_Identical_(OP, OP *);
OP primitiveProtoObject_Class(OP, OP *);
OP primitiveObject_At_(OP, OP *);
OP primitiveObject_At_Put_(OP, OP *);
OP primitiveObject_Size(OP, OP *);
OP primitiveObject_ByteAt_(OP, OP *);
OP primitiveObject_ByteAt_Put_(OP, OP *);
OP primitiveObject_ByteSize(OP, OP *);
OP primitiveObject_ShallowCopy(OP, OP *);
OP primitiveObject_InspectToLevel_(OP, OP *);

#endif
