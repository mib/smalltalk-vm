#include <stdio.h> /* FILE, stdin, stdout, fopen, fclose, putchar */
#include <stdlib.h> /* calloc, free */
#include <string.h> /* memcpy */
#include "common.h"
#include "constants.h"
#include "primitives-fileStreams.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"


FILE ** fileStreams;
int fileStreamsSize = FILE_STREAM_TABLE_INITIAL_SIZE;
int fileStreamsCount = 0;


void allocateFileStreamTable() {
	// calloc will set the memory to zero (equals to NULL)
	fileStreams = (FILE **) calloc(fileStreamsSize, sizeof(*fileStreams));
}

void deallocateFileStreamTable() {
	// close streams that were left open
	for(int index = 0; index < fileStreamsSize; ++index) {
		if(fileStreams[index] != NULL) {
			fclose(fileStreams[index]);
		}
	}
	
	free(fileStreams);
}

void enlargeFileStreamTable() {
	FILE ** oldFileStreams = fileStreams;
	int oldFileStreamsSize = fileStreamsSize;
	
	fileStreamsSize <<= 1;
	
	allocateFileStreamTable();
	
	memcpy(fileStreams, oldFileStreams, sizeof(*fileStreams) * oldFileStreamsSize);
	
	free(oldFileStreams);
}


// Transcript class>>#nextPut:
OP primitiveTranscriptClass_NextPut_(OP receiver, OP * arguments) {
	putchar(characterValueOf(arguments[0]));
	
	return arguments[0];
}

// FileStream>>#open
OP primitiveFileStream_Open(OP receiver, OP * arguments) {
	OP filenameOp = fetchNamedPointer(receiver, FILESTREAM_FILENAME);
	OP forReadOp = fetchNamedPointer(receiver, FILESTREAM_FOR_READ);
	OP forWriteOp = fetchNamedPointer(receiver, FILESTREAM_FOR_WRITE);
	
	char * filename = (char *) stringToCharArray(filenameOp);
	
	FILE * descriptor;
	
	// stdin or stdout is specified by "-", otherwise it's a standard filename;
	// to specify a file literally named "-" one can use the form "./-"
	if(strcmp(filename, "-") == 0) {
		if(forReadOp == OBJECT_TRUE_OP && forWriteOp == OBJECT_FALSE_OP) {
			descriptor = stdin;
		}
		else if(forWriteOp == OBJECT_TRUE_OP && forReadOp == OBJECT_FALSE_OP) {
			descriptor = stdout;
		}
		else {
			// cannot open for both read and write
			free(filename);
			return PRIMITIVE_FAIL_OP;
		}
	}
	else {
		descriptor = fopen(filename,
		                   (forReadOp == OBJECT_TRUE_OP && forWriteOp == OBJECT_TRUE_OP)
		                    ? "w+"
		                    : ((forReadOp == OBJECT_TRUE_OP)
		                        ? "r"
		                        : "w"));
	}
	free(filename);
	
	if(descriptor == NULL) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int index = helperPrimitiveFileStream_Index(receiver);
	
	fileStreams[index] = descriptor;
	
	storeNamedSmallInteger(receiver, FILESTREAM_DESCRIPTOR, index);
	
	return receiver;
}

// FileStream>>#next
OP primitiveFileStream_Next(OP receiver, OP * arguments) {
	OP indexOp = fetchNamedPointer(receiver, FILESTREAM_DESCRIPTOR);
	
	if(indexOp == OBJECT_NIL_OP) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int index = smallIntegerValueOf(indexOp);
	
	if(fileStreams[index] == NULL) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int character = fgetc(fileStreams[index]);
	
	return (character != EOF) ? characterObjectOf(character) : OBJECT_NIL_OP;
}

// FileStream>>#atEnd
OP primitiveFileStream_AtEnd(OP receiver, OP * arguments) {
	OP indexOp = fetchNamedPointer(receiver, FILESTREAM_DESCRIPTOR);
	
	if(indexOp == OBJECT_NIL_OP) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int index = smallIntegerValueOf(indexOp);
	
	if(fileStreams[index] == NULL) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int character = fgetc(fileStreams[index]);
	
	if(character == EOF) {
		return OBJECT_TRUE_OP;
	}
	
	ungetc(character, fileStreams[index]);
	
	return OBJECT_FALSE_OP;
}

// FileStream>>#nextPut:
OP primitiveFileStream_NextPut_(OP receiver, OP * arguments) {
	OP indexOp = fetchNamedPointer(receiver, FILESTREAM_DESCRIPTOR);
	
	if(indexOp == OBJECT_NIL_OP) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int index = smallIntegerValueOf(indexOp);
	
	if(fileStreams[index] == NULL) {
		return PRIMITIVE_FAIL_OP;
	}
	
	fputc(characterValueOf(arguments[0]), fileStreams[index]);
	
	return arguments[0];
}

// FileStream>>#close
OP primitiveFileStream_Close(OP receiver, OP * arguments) {
	OP indexOp = fetchNamedPointer(receiver, FILESTREAM_DESCRIPTOR);
	
	if(indexOp == OBJECT_NIL_OP) {
		return PRIMITIVE_FAIL_OP;
	}
	
	int index = smallIntegerValueOf(indexOp);
	
	if(fileStreams[index] == NULL) {
		return PRIMITIVE_FAIL_OP;
	}
	
	fclose(fileStreams[index]);
	fileStreams[index] = NULL;
	
	storeNamedPointer(receiver, FILESTREAM_DESCRIPTOR, OBJECT_NIL_OP);
	
	return receiver;
}

int helperPrimitiveFileStream_Index(OP receiver) {
	if((fileStreamsSize - fileStreamsCount) * FILE_STREAM_TABLE_FREE_SPACE < fileStreamsSize) {
		enlargeFileStreamTable();
	}
	
	int index = receiver % fileStreamsSize;
	
	while(fileStreams[index] != NULL) {
		if(++index >= fileStreamsSize) {
			index = 0;
		}
	}
	
	return index;
}
