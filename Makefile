CFLAGS=-g -std=c99
OBJS=src/main.o src/common.o src/vm.o src/objectMemory.o src/objectMemory-gc.o src/objectMemory-gc-remembered.o src/objectMemory-objects.o src/objectMemory-smallIntegers.o src/objectMemory-characters.o src/interpreter.o src/interpreter-instructions.o src/interpreter-sends.o src/context-stack.o src/context-helpers.o src/context-changing.o src/context-creation.o src/primitives.o src/debugTools.o

alpha: $(OBJS)
	$(CC) -o vm $(OBJS)

clean:
	-@rm -f vm $(OBJS)

$(OBJS): src/common.h src/constants.h
