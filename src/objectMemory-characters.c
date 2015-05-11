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
