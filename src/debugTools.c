#include "common.h"
#include "constants.h"
#include "debugTools.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"


void inspect(OP op) {
	deepInspect(op, 1);
}
void finspect(FILE * stream, OP op) {
	fdeepInspect(stream, op, 1);
}

void deepInspect(OP op, int depth) {
	deepInspectOnLevel(op, depth, 0);
}
void fdeepInspect(FILE * stream, OP op, int depth) {
	fdeepInspectOnLevel(stream, op, depth, 0);
}

void deepInspectOnLevel(OP op, int depth, int level) {
	fdeepInspectOnLevel(stdout, op, depth, level);
}
void fdeepInspectOnLevel(FILE * stream, OP op, int depth, int level) {
	if(depth < 0) {
		putc('\n', stream);
		return;
	}
	
	if(op == OBJECT_NIL_OP) {
		fprintf(stream, "nil\n");
		return;
	}
	if(op == OBJECT_TRUE_OP) {
		fprintf(stream, "true\n");
		return;
	}
	if(op == OBJECT_FALSE_OP) {
		fprintf(stream, "false\n");
		return;
	}
	if(isSmallIntegerObject(op)) {
		fprintf(stream, "%d\n", smallIntegerValueOf(op));
		return;
	}
	if(isCharacterObject(op)) {
		putc('$', stream);
		int value = characterValueOf(op);
		if((value >= 33 && value <= 126) || value >= 161) {
			putc(value, stream);
			putc('\n', stream);
		}
		else {
			fprintf(stream, " [%d]\n", value);
		}
		return;
	}
	
	OP classOp = fetchClass(op);
	
	if(classOp == STRING_OP) {
		putc('\'', stream);
		fprintString(stream, op);
		putc('\'', stream);
		putc('\n', stream);
		return;
	}
	if(classOp == SYMBOL_OP) {
		putc('#', stream);
		fprintString(stream, op);
		putc('\n', stream);
		return;
	}
	
	if(fetchClass(classOp) == METACLASS_OP) {
		fprintString(stream, fetchNamedPointer(op, CLASS_NAME));
	}
	else if(fetchClass(classOp) == METACLASS_CLASS_OP) {
		fprintString(stream, fetchNamedPointer(fetchNamedPointer(op, METACLASS_CLASS), CLASS_NAME));
		fprintf(stream, " class");
	}
	else {
		fprintf(stream, "a ");
		fprintString(stream, fetchNamedPointer(classOp, CLASS_NAME));
	}
	fprintf(stream, " (op = %d) \n", op);
	
	if(depth > 1) {
		fdeepInspectNamedPointers(stream, op, classOp, depth - 1, level);
		fdeepInspectIndexablePointers(stream, op, depth - 1, level);
		fdeepInspectIndexableBytes(stream, op, depth - 1, level);
	}
}

int fdeepInspectNamedPointers(FILE * stream, OP op, OP classOp, int depth, int level) {
	if(classOp == OBJECT_NIL_OP) {
		return 0;
	}
	
	int offset = fdeepInspectNamedPointers(stream, op, fetchNamedPointer(classOp, BEHAVIOR_SUPERCLASS), depth, level);
	
	OP namedPointerNames = fetchNamedPointer(classOp, CLASSDESCRIPTION_INSTANCE_VARIABLES);
	int namedPointerLength = fetchIndexablePointerLength(namedPointerNames);
	
	for(int index = 0; index < namedPointerLength; ++index) {
		for(int i = 0; i < level; ++i) putc(' ', stream);
		fprintf(stream, "- ");
		OP name = fetchIndexablePointer(namedPointerNames, index);
		fprintString(stream, name);
		fprintf(stream, ": ");
		fdeepInspectOnLevel(stream, fetchNamedPointer(op, offset + index), depth - 1, level + stringLength(name) + 4);
	}
	
	return offset + namedPointerLength;
}

void fdeepInspectIndexablePointers(FILE * stream, OP op, int depth, int level) {
	int indexablePointerLength = fetchIndexablePointerLength(op);
	
	for(int index = 0; index < indexablePointerLength; ++index) {
		for(int i = 0; i < level; ++i) putc(' ', stream);
		fprintf(stream, "- %d: ", index + 1);
		int indexLength = 1 + (index >= 10) + (index >= 100) + (index >= 1000) + (index >= 10000);
		fdeepInspectOnLevel(stream, fetchIndexablePointer(op, index), depth - 1, level + indexLength + 4);
	}
}

void fdeepInspectIndexableBytes(FILE * stream, OP op, int depth, int level) {
	int indexableByteLength = fetchIndexableByteLength(op);
	
	if(indexableByteLength == 0) {
		return;
	}
	
	for(int index = 0; index < indexableByteLength; ++index) {
		for(int i = 0; i < level; ++i) putc(' ', stream);
		fprintf(stream, "- byte %d: %d\n", index + 1, fetchIndexableByte(op, index));
	}
}

int stringLength(OP op) {
	return fetchIndexablePointerLength(op);
}

void printString(OP op) {
	fprintString(stdout, op);
}
void fprintString(FILE * stream, OP op) {
	if(fetchClass(op) != SYMBOL_OP && fetchClass(op) != STRING_OP) {
		fprintf(stream, "(not a string)");
		return;
	}
	
	int length = fetchIndexablePointerLength(op);
	
	for(int i = 0; i < length; ++i) {
		putc(characterValueOf(fetchIndexablePointer(op, i)), stream);
	}
}
