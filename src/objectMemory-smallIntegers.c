#include "common.h"
#include "constants.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-objects.h"
#include "primitives.h"


int smallIntegerValueOf(OP op) {
	if(!isSmallIntegerObject(op)) {
		error("Not a small integer.");
	}
	
	// convert a negative number in a 32-bit object pointer to a 64-bit integer correctly
	if(op & 0x80000000) {
		return ((-1 << 31) | op) >> 1; // 31 is to avoid warnings when integer is exactly 32-bit; the last bit is 1 anyway (as it is a negative number) so it does not matter
	}
	
	return op >> 1;
}

OP smallIntegerObjectOf(int value) {
	if(!isSmallIntegerValue(value)) {
		error("Not in range.");
	}
	
	return (value << 1) | 1;
}

int isSmallIntegerObject(OP op) {
	return op & 1;
}

int isSmallIntegerValue(int value) {
	// it has to fit in 31 bits
	return value >= -0x40000000 && value <= 0x3fffffff;
}


/* small integer helper functions */

int fetchNamedSmallInteger(OP op, int fieldIndex) {
	return smallIntegerValueOf(fetchNamedPointer(op, fieldIndex));
}

void storeNamedSmallInteger(OP op, int fieldIndex, int value) {
	storeNamedPointer(op, fieldIndex, smallIntegerObjectOf(value));
}

int incrementNamedSmallInteger(OP op, int fieldIndex) {
	return addToNamedSmallInteger(op, fieldIndex, 1);
}
int decrementNamedSmallInteger(OP op, int fieldIndex) {
	return addToNamedSmallInteger(op, fieldIndex, -1);
}
int addToNamedSmallInteger(OP op, int fieldIndex, int diff) {
	OP primitiveArguments[1] = {smallIntegerObjectOf(diff)};
	OP value = primitiveSmallInteger_Add_(fetchNamedPointer(op, fieldIndex), primitiveArguments);
	
	if(value == PRIMITIVE_FAIL_OP) {
		error("addToNamedSmallInteger: primitive failed\n");
	}
	
	storeNamedPointer(op, fieldIndex, value);
	
	return smallIntegerValueOf(value);
}
