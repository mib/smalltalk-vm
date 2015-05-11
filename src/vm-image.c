#include <stdio.h> /* FILE, fopen, ftell, fseek, rewind, fread, fclose */
#include <stdlib.h> /* malloc, free */
#include <string.h> /* memcpy */
#include "common.h"
#include "constants.h"
#include "vm-image.h"
#include "vm.h"
#include "objectMemory.h"


void loadImageFile(char * filename) {
	FILE * imageFile = fopen(filename, "rb");
	
	if(imageFile == NULL) {
		error("Could not load image file.");
	}
	
	// load the whole image to memory so that we don't read byte after byte
	
	fseek(imageFile, 0L, SEEK_END);
	long filesize = ftell(imageFile);
	rewind(imageFile);
	
	int chunkSize = 4096;
	unsigned char * image = (unsigned char *) malloc(sizeof(*image) * (filesize + chunkSize));
	unsigned char * imageReadPosition = image;
	
	while(fread(imageReadPosition, 1, chunkSize, imageFile) == chunkSize) {
		imageReadPosition += chunkSize;
	}
	
	fclose(imageFile);
	
	OBJ object;
	OP objectPointer;
	unsigned char * objectBytes;
	int objectSize;
	int index;
	int byteStart;
	int byteSize;
	
	unsigned char * imagePosition = image;
	
	// at the end of the image there is a zero (a dummy object with size = 0)
	while((objectSize = getImageField(imagePosition)) != 0) {
		imagePosition += OBJECT_FIELD_BYTES;
		
		OP objectPointer = getImageField(imagePosition);
		imagePosition += OBJECT_FIELD_BYTES;
		
		object = allocateOldObjectOfSizeAtPosition(objectSize, objectPointer >> 2);
		
		*object = objectSize;
		*(object + 1) = objectPointer;
		
		for(index = 2; index < OBJECT_HEADER_LENGTH; ++index) {
			*(object + index) = getImageField(imagePosition);
			imagePosition += OBJECT_FIELD_BYTES;
		}
		
		byteStart = *(object + OBJECT_INDEXABLE_BYTE_FIRST_FIELD);
		byteSize = (*(object + OBJECT_INDEXABLE_BYTE_SIZE_FIELD) + OBJECT_FIELD_BYTES - 1) / OBJECT_FIELD_BYTES * OBJECT_FIELD_BYTES;
		
		for(index = OBJECT_HEADER_LENGTH; index < byteStart; ++index) {
			*(object + index) = getImageField(imagePosition);
			imagePosition += OBJECT_FIELD_BYTES;
		}
		
		// bytes are copied as-is so that they are always in order, even if
		// the current platform stores LSB first
		if(byteSize) {
			objectBytes = (unsigned char *) (object + byteStart);
			
			memcpy(objectBytes, imagePosition, sizeof(*objectBytes) * byteSize);
			
			imagePosition += byteSize;
		}
	}
	
	free(image);
}

int getImageField(unsigned char * image) {
	int byte1 = image[0];
	int byte2 = image[1];
	int byte3 = image[2];
	int byte4 = image[3];
	
	// correctly return -1 in case sizeof(int) > 4 bytes;
	// there are no other plain negative integers in the image file
	if((byte1 & byte2 & byte3 & byte4) == 0xff) {
		return -1;
	}
	
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}
