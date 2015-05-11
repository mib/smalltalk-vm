#include <stdio.h> /* printf, putchar */
#include "common.h"
#include "constants.h"
#include "debugTools.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"


void inspect(OP op) {
	deepInspect(op, 1);
}

void deepInspect(OP op, int depth) {
	deepInspectOnLevel(op, depth, 0);
}

void deepInspectOnLevel(OP op, int depth, int level) {
	if(depth < 0) {
		putchar('\n');
		return;
	}
	
	if(op == OBJECT_NIL_OP) {
		printf("nil\n");
		return;
	}
	if(op == OBJECT_TRUE_OP) {
		printf("true\n");
		return;
	}
	if(op == OBJECT_FALSE_OP) {
		printf("false\n");
		return;
	}
	if(isSmallIntegerObject(op)) {
		printf("%d\n", smallIntegerValueOf(op));
		return;
	}
	if(isCharacterObject(op)) {
		putchar('$');
		int value = characterValueOf(op);
		if((value >= 33 && value <= 126) || value >= 161) {
			putchar(value);
			putchar('\n');
		}
		else {
			printf(" [%d]\n", value);
		}
		return;
	}
	
	OP classOp = fetchClass(op);
	
	if(classOp == STRING_OP) {
		putchar('\'');
		printString(op);
		putchar('\'');
		putchar('\n');
		return;
	}
	if(classOp == SYMBOL_OP) {
		putchar('#');
		printString(op);
		putchar('\n');
		return;
	}
	
	if(fetchClass(classOp) == METACLASS_OP) {
		printString(fetchNamedPointer(op, CLASS_NAME));
	}
	else if(fetchClass(classOp) == METACLASS_CLASS_OP) {
		printString(fetchNamedPointer(fetchNamedPointer(op, METACLASS_CLASS), CLASS_NAME));
		printf(" class");
	}
	else {
		printf("a ");
		printString(fetchNamedPointer(classOp, CLASS_NAME));
	}
	printf("\n");
	
	if(depth > 1) {
		deepInspectNamedPointers(op, classOp, depth - 1, level);
		deepInspectIndexablePointers(op, depth - 1, level);
		deepInspectIndexableBytes(op, depth - 1, level);
	}
}

int deepInspectNamedPointers(OP op, OP classOp, int depth, int level) {
	if(classOp == OBJECT_NIL_OP) {
		return 0;
	}
	
	int offset = deepInspectNamedPointers(op, fetchNamedPointer(classOp, BEHAVIOR_SUPERCLASS), depth, level);
	
	OP namedPointerNames = fetchNamedPointer(classOp, CLASSDESCRIPTION_INSTANCE_VARIABLES);
	int namedPointerLength = fetchIndexablePointerLength(namedPointerNames);
	
	for(int index = 0; index < namedPointerLength; ++index) {
		for(int i = 0; i < level; ++i) putchar(' ');
		printf("- ");
		OP name = fetchIndexablePointer(namedPointerNames, index);
		printString(name);
		printf(": ");
		deepInspectOnLevel(fetchNamedPointer(op, offset + index), depth - 1, level + stringLength(name) + 4);
	}
	
	return offset + namedPointerLength;
}

void deepInspectIndexablePointers(OP op, int depth, int level) {
	int indexablePointerLength = fetchIndexablePointerLength(op);
	
	for(int index = 0; index < indexablePointerLength; ++index) {
		for(int i = 0; i < level; ++i) putchar(' ');
		printf("- %d: ", index + 1);
		int indexLength = 1 + (index >= 10) + (index >= 100) + (index >= 1000) + (index >= 10000);
		deepInspectOnLevel(fetchIndexablePointer(op, index), depth - 1, level + indexLength + 4);
	}
}

void deepInspectIndexableBytes(OP op, int depth, int level) {
	int indexableByteLength = fetchIndexableByteLength(op);
	
	if(indexableByteLength == 0) {
		return;
	}
	
	for(int index = 0; index < indexableByteLength; ++index) {
		for(int i = 0; i < level; ++i) putchar(' ');
		printf("- byte %d: %d\n", index + 1, fetchIndexableByte(op, index));
	}
}

int stringLength(OP op) {
	return fetchIndexablePointerLength(op);
}

void printString(OP op) {
	if(fetchClass(op) != SYMBOL_OP && fetchClass(op) != STRING_OP) {
		printf("(not a string)");
		return;
	}
	
	int length = fetchIndexablePointerLength(op);
	
	for(int i = 0; i < length; ++i) {
		putchar(characterValueOf(fetchIndexablePointer(op, i)));
	}
}
