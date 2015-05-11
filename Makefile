CFLAGS=-std=c99
OBJS=src/main.o src/common.o src/vm.o src/vm-image.o src/vm-script.o src/vm-repl.o src/objectMemory.o src/objectMemory-gc.o src/objectMemory-gc-old.o src/objectMemory-gc-remembered.o src/objectMemory-objects.o src/objectMemory-smallIntegers.o src/objectMemory-characters.o src/interpreter.o src/interpreter-instructions.o src/interpreter-instructions-closure.o src/interpreter-instructions-jump.o src/interpreter-instructions-push.o src/interpreter-instructions-return.o src/interpreter-instructions-send.o src/interpreter-instructions-stack.o src/interpreter-instructions-store.o src/interpreter-sends.o src/interpreter-sends-lookup.o src/context-stack.o src/context-stack-active.o src/context-helpers.o src/context-changing.o src/context-creation.o src/primitives.o src/primitives-objects.o src/primitives-arithmetics.o src/primitives-fileStreams.o src/debugTools.o

alpha: $(OBJS)
	$(CC) -o vm $(OBJS)

clean:
	-@rm -f vm $(OBJS)

$(OBJS): src/common.h src/constants.h
