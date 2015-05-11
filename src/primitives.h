#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

OP primitiveBehavior_BasicNew(OP, OP *);
OP primitiveBehavior_BasicNew_(OP, OP *);
OP primitiveBehavior_BasicNew_Bytes_(OP, OP *);
OP primitiveBehavior_AddSelector_WithMethod_(OP, OP *);
OP primitiveContext_Resume_(OP, OP *);
OP primitiveContext_Retry(OP, OP *);
OP primitiveBlockClosure_Value(OP, OP *);
OP primitiveBlockClosure_Value_(OP, OP *);
OP primitiveBlockClosure_Value_Value_(OP, OP *);
OP helperPrimitiveBlockClosure_ValueWithArguments(OP, OP *, int);
OP primitiveSystemDictionary_At_(OP, OP *);
OP primitiveSystemDictionary_At_Put_(OP, OP *);
OP primitiveSystemDictionary_Size(OP, OP *);

#endif
