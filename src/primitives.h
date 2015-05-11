#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include <stdio.h>

OP primitiveProtoObject_Identical_(OP, OP *);
OP primitiveProtoObject_Class(OP, OP *);
OP primitiveBehavior_BasicNew(OP, OP *);
OP primitiveBehavior_BasicNew_(OP, OP *);
OP primitiveBehavior_BasicNew_Bytes_(OP, OP *);
OP primitiveObject_At_(OP, OP *);
OP primitiveObject_At_Put_(OP, OP *);
OP primitiveObject_Size(OP, OP *);
OP primitiveObject_ByteAt_(OP, OP *);
OP primitiveObject_ByteAt_Put_(OP, OP *);
OP primitiveObject_ByteSize(OP, OP *);
OP primitiveSmallInteger_Add_(OP, OP *);
OP primitiveSmallInteger_Subtract_(OP, OP *);
OP primitiveSmallInteger_Times_(OP, OP *);
OP primitiveSmallInteger_IntegerDivide_(OP, OP *);
OP primitiveSmallInteger_Modulo_(OP, OP *);
OP primitiveSmallInteger_Equal_(OP, OP *);
OP primitiveSmallInteger_NotEqual_(OP, OP *);
OP primitiveSmallInteger_LessThan_(OP, OP *);
OP primitiveSmallInteger_LessThanOrEqual_(OP, OP *);
OP primitiveSmallInteger_GreaterThan_(OP, OP *);
OP primitiveSmallInteger_GreaterThanOrEqual_(OP, OP *);
OP primitiveContext_Resume_(OP, OP *);
OP primitiveContext_Retry(OP, OP *);
OP primitiveBlockClosure_Value(OP, OP *);
OP primitiveBlockClosure_Value_(OP, OP *);
OP primitiveBlockClosure_Value_Value_(OP, OP *);
OP helperPrimitiveBlockClosure_ValueWithArguments(OP, OP *, int);
OP primitiveTranscriptClass_NextPut_(OP, OP *);
OP primitiveSystemDictionary_At_(OP, OP *);
OP primitiveSystemDictionary_At_Put_(OP, OP *);
OP primitiveSystemDictionary_Size(OP, OP *);
OP primitiveFileStreamClass_OpenAndReadFrom_(OP, OP *);
OP primitiveFileStream_Close(OP, OP *);
OP primitiveFileStream_Flush(OP, OP *);

#endif
