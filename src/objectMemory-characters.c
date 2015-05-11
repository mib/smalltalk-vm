#include <stdlib.h> /* malloc */
#include "common.h"
#include "constants.h"
#include "objectMemory-characters.h"
#include "objectMemory.h"


OP characterObjectOf(int character) {
	return OBJECT_CHARACTER_0_OP + ((character * (OBJECT_HEADER_LENGTH + 1)) << 2);
}

int characterValueOf(OP op) {
	return ((op - OBJECT_CHARACTER_0_OP) >> 2) / (OBJECT_HEADER_LENGTH + 1);
}

int isCharacterObject(OP op) {
	return op >= characterObjectOf(0)
		&& op <= characterObjectOf(255)
		&& isOldObject(op);
}

unsigned char * stringToCharArray(OP op) {
	int length = fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_SIZE_FIELD);
	
	unsigned char * array = (char *) malloc(sizeof(*array) * (length + 1)); // +1 = '\0'
	
	for(int i = 0; i < length; ++i) {
		array[i] = characterValueOf(fetchIndexablePointer(op, i));
	}
	
	array[length] = '\0';
	
	return array;
}
