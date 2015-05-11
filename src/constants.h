#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define OP int
#define OBJ int*


// compile macros instead of heavily called short functions to achieve much better performance
#define OPTIMIZED_MACROS


// special program counter / bytecode value to halt the interpreter
// when the topmost context returns
#define INTERPRETER_EOF -1


#define NOTHING 0
#define IMAGE_FILE 1
#define SCRIPT_FILE 2


// young space size will be divided in two halves (new and survivor)
// the smallest possible is 2000 (~ 0x800)
// 512K * sizeof(int)
#define DEFAULT_YOUNG_SPACE_SIZE 0x80000

// the smallest possible old space size is (size of the image file in bytes / 4)
// plus space needed to accomodate command-line arguments (12 + sum(10 + argument string length))
// 64M * sizeof(int)
#define DEFAULT_OLD_SPACE_SIZE 0x4000000

// resolved selectors will be put into a cache for faster consecutive lookups;
// this controls how many selectors will be stored for each class
#define METHOD_LOOKUP_CACHE_ENTRY_SIZE 10
// this controls how many class entries will be allocated initially
#define METHOD_LOOKUP_CACHE_INITIAL_CLASSES 2000
// the cache size is doubled is the free space drops below this value
#define METHOD_LOOKUP_CACHE_FREE_SPACE 500

// opened file streams are saved in a table for an index-to-file-descriptor mapping;
// this controls how many file descriptor entries will be allocated initially
#define FILE_STREAM_TABLE_INITIAL_SIZE 100
// the table size is doubled is the free space drops below 1/X * 100 %, where X is this value
#define FILE_STREAM_TABLE_FREE_SPACE 3


#define NOT_REMEMBERED -1
#define NO_NEXT_REMEMBERED -2
#define ALREADY_COPIED -1

// one field of an object (either OP or a plain integer) equals to 4 bytes
#define OBJECT_FIELD_BYTES 4

// header fields of an object
#define OBJECT_HEADER_LENGTH 10
#define OBJECT_SIZE_FIELD 0
#define OBJECT_OP_FIELD 1
// young objects have age, old objects have a pointer to the next remembered object
#define OBJECT_AGE_FIELD 2
#define OBJECT_NEXT_REMEMBERED_INDEX_FIELD 2
#define OBJECT_CLASS_FIELD 3
#define OBJECT_NAMED_POINTER_FIRST_FIELD 4
#define OBJECT_NAMED_POINTER_SIZE_FIELD 5
#define OBJECT_INDEXABLE_POINTER_FIRST_FIELD 6
#define OBJECT_INDEXABLE_POINTER_SIZE_FIELD 7
#define OBJECT_INDEXABLE_BYTE_FIRST_FIELD 8
#define OBJECT_INDEXABLE_BYTE_SIZE_FIELD 9

// class variables start at sixth field in a class (the first five are basic class instance fields)
#define CLASS_VARIABLE_OFFSET 5


// hardcoded OPs of classes and objects that are needed throughout the VM

// basic objects
#define OBJECT_NIL_OP        (0 << 2)
#define OBJECT_TRUE_OP       (10 << 2)
#define OBJECT_FALSE_OP      (20 << 2)
#define OBJECT_SMALLTALK_OP  (30 << 2)

// class and metaclass hierarchy
#define METACLASS_OP         (87 << 2)
#define OBJECT_OP            (117 << 2)
#define METACLASS_CLASS_OP   (177 << 2)

// basic classes
#define COMPILEDMETHOD_OP    (222 << 2)
#define METHODCONTEXT_OP     (237 << 2)
#define BLOCKCLOSURE_OP      (252 << 2)
#define MESSAGE_OP           (267 << 2)
#define UNDEFINEDOBJECT_OP   (297 << 2)
#define SMALLINTEGER_OP      (312 << 2)
#define ARRAY_OP             (327 << 2)
#define CHARACTER_OP         (342 << 2)
#define STRING_OP            (361 << 2)
#define SYMBOL_OP            (376 << 2)
#define COMPILER_OP          (392 << 2)
#define SCRIPTRUNNER_OP      (499 << 2)

// symbols and message selectors
#define OBJECT_DOES_NOT_UNDERSTAND_SELECTOR_OP                (3350 << 2)
#define OBJECT_COMPILE_SOURCE_FOR_CLASS_SELECTOR_OP           (3378 << 2)
#define OBJECT_DEFAULT_EXCEPTION_HANDLER_CONTEXT_SELECTOR_OP  (3411 << 2)
#define OBJECT_ARGUMENTS_SELECTOR_OP                          (3451 << 2)
#define OBJECT_PRINT_STRING_SELECTOR_OP                       (3471 << 2)

// characters
#define OBJECT_CHARACTER_0_OP      (515 << 2)
//#define OBJECT_CHARACTER_1_OP    (526 << 2)
//#define OBJECT_CHARACTER_2_OP    (537 << 2)
// ...
//#define OBJECT_CHARACTER_255_OP  (3320 << 2)

// small integers
// they are defined in objectMemory-smallIntegers.c, these constants are used
// in optimized instructions (push 1, push 0, push -1)
#define OBJECT_SMALLINTEGER_1_OP  3
#define OBJECT_SMALLINTEGER_0_OP  1
#define OBJECT_SMALLINTEGER_NEG_1_OP  -1

// special, non-existent OPs used as a result of some primitive calls;
// `nil` spans from 0 to 9 in the old space (its OP == 0 and only has a header, no instance fields)
// so 1 through 9 will never be used
#define PRIMITIVE_FAIL_OP (1 << 2)
#define PRIMITIVE_VOID_OP (2 << 2)


// fixed instance variable indexes
#define DICTIONARY_KEY_ARRAY 0
#define DICTIONARY_VALUE_ARRAY 1
#define BEHAVIOR_SUPERCLASS 0
#define BEHAVIOR_METHODS 1
#define CLASSDESCRIPTION_INSTANCE_VARIABLES 2
#define CLASSDESCRIPTION_TOTAL_INSTANCE_VARIABLE_SIZE 3
#define CLASS_NAME 4
#define METACLASS_CLASS 4
#define MESSAGE_SELECTOR 0
#define MESSAGE_ARGUMENTS 1
#define MESSAGE_LOOKUP_CLASS 2
#define COMPILEDMETHOD_PRIMITIVE 0
#define COMPILEDMETHOD_PARAMETER_COUNT 1
#define COMPILEDMETHOD_TEMPORARY_COUNT 2
#define COMPILEDMETHOD_STACK_SIZE 3
#define COMPILEDMETHOD_CLASS 4
#define COMPILEDMETHOD_CONTEXT_TEMPLATE 5
#define METHODCONTEXT_SENDER 0
#define METHODCONTEXT_PROGRAM_COUNTER 1
#define METHODCONTEXT_STACK_POINTER 2
#define METHODCONTEXT_RECEIVER 3
#define METHODCONTEXT_METHOD 4
#define METHODCONTEXT_CLOSURE 5
#define METHODCONTEXT_OUTER_CONTEXT 6
#define METHODCONTEXT_SELECTOR 7
#define BLOCKCLOSURE_OUTER_CONTEXT 0
#define BLOCKCLOSURE_INITIAL_PROGRAM_COUNTER 1
#define BLOCKCLOSURE_PARAMETER_COUNT 2
#define BLOCKCLOSURE_TEMPORARY_COUNT 3
#define BLOCKCLOSURE_VARIABLE_OFFSET 4
#define BLOCKCLOSURE_STACK_SIZE 5
#define BLOCKCLOSURE_CONTEXT_TEMPLATE 6
#define POSITIONABLESTREAM_COLLECTION 0
#define FILESTREAM_FILENAME 0
#define FILESTREAM_FOR_READ 1
#define FILESTREAM_FOR_WRITE 2
#define FILESTREAM_DESCRIPTOR 3

#define INSTRUCTION_RETURN_SELF_FROM_METHOD 63

#endif
