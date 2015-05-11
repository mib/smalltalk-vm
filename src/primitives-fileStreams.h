#ifndef __PRIMITIVES_FILESTREAMS_H__
#define __PRIMITIVES_FILESTREAMS_H__

#include <stdio.h>

void allocateFileStreamTable();
void deallocateFileStreamTable();
void enlargeFileStreamTable();
OP primitiveTranscriptClass_NextPut_(OP, OP *);
OP primitiveFileStream_Open(OP, OP *);
OP primitiveFileStream_Next(OP, OP *);
OP primitiveFileStream_AtEnd(OP, OP *);
OP primitiveFileStream_NextPut_(OP, OP *);
OP primitiveFileStream_Close(OP, OP *);
int helperPrimitiveFileStream_Index(OP);

#endif
