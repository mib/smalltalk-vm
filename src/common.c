#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "constants.h"
#include "vm.h"


#ifdef OPTIMIZED_MACROS

///#include "constants.h"

// from: objectMemory.c
///OBJ youngSpace;
///OBJ youngSpaceEnd;
///OBJ newSpace;
///OBJ newSpaceEnd;
///OBJ nextFreeNewObject;
///OBJ survivorSpace;
///OBJ survivorSpaceEnd;
///OBJ oldSpace;
///OBJ oldSpaceEnd;
///OBJ oldSpaceSafeEnd;
///#define isNewObject(op) (!isSmallIntegerObject(op) && objectFor(op) >= newSpace && objectFor(op) < newSpaceEnd)
///#define isSurvivorObject(op) (!isSmallIntegerObject(op) && objectFor(op) >= survivorSpace && objectFor(op) < survivorSpaceEnd)
///#define isYoungObject(op) (((op) & 3) == 2)
///#define isOldObject(op) (((op) & 3) == 0)
///#define objectFor(op) (((op) & 3) == 0 ? oldSpace + ((op) >> 2) : youngSpace + ((op) >> 2))
///#define objectPointerFor(object) (((object) >= oldSpace && (object) < oldSpaceEnd) ? (((object) - oldSpace) << 2) : ((((object) - youngSpace) << 2) | 2))
///#define hasNewSpaceAtLeast(size) (nextFreeNewObject + size < newSpaceEnd)

// from objectMemory-objects.c
///#include "objectMemory-objects.h"
///#include "objectMemory-gc-remembered.h"
///#define fetchFieldOf(op, fieldIndex) (*(objectFor(op) + (fieldIndex)))
///#define storeFieldOf(op, fieldIndex, valueOp) (*(objectFor(op) + (fieldIndex)) = valueOp)
///#define fetchObjectFieldOf(object, fieldIndex) (*((object) + (fieldIndex)))
///#define storeObjectFieldOf(object, fieldIndex, valueOp) (*((object) + (fieldIndex)) = valueOp)
///#define fetchNamedPointer(op, fieldIndex) (fetchFieldOf(op, fetchFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD) + (fieldIndex)))
///#define fetchIndexablePointer(op, fieldIndex) (fetchFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + (fieldIndex)))
///#define storeNamedPointer(op, fieldIndex, valueOp) ((isNewObject(valueOp) && !isNewObject(op)) ? addToRemembered(op) : 0, storeFieldOf(op, fetchFieldOf(op, OBJECT_NAMED_POINTER_FIRST_FIELD) + (fieldIndex), valueOp))
///#define storeIndexablePointer(op, fieldIndex, valueOp) ((isNewObject(valueOp) && !isNewObject(op)) ? addToRemembered(op) : 0, storeFieldOf(op, fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_FIRST_FIELD) + (fieldIndex), valueOp))
///#define fetchClass(op) (isSmallIntegerObject(op) ? SMALLINTEGER_OP : fetchFieldOf(op, OBJECT_CLASS_FIELD))
///#define fetchNamedPointerLength(op) (fetchFieldOf(op, OBJECT_NAMED_POINTER_SIZE_FIELD))
///#define fetchIndexablePointerLength(op) (fetchFieldOf(op, OBJECT_INDEXABLE_POINTER_SIZE_FIELD))
///#define fetchObjectPointer(op) (isSmallIntegerObject(op) ? (op) : fetchFieldOf(op, OBJECT_OP_FIELD))
///#define fetchSize(op) (isSmallIntegerObject(op) ? OBJECT_HEADER_LENGTH : fetchFieldOf(op, OBJECT_SIZE_FIELD))
///#define instantiateClass(class, indexablePointerSize, indexableByteSize) (instantiateClassInSpace(class, indexablePointerSize, indexableByteSize, 0))
///#define instantiateClassInOldSpace(class, indexablePointerSize, indexableByteSize) (instantiateClassInSpace(class, indexablePointerSize, indexableByteSize, 1))

// from objectMemory-smallIntegers.c
///#define isSmallIntegerObject(op) (op & 1)
///#define isSmallIntegerValue(value) (value >= -0x40000000 && value <= 0x3fffffff)
///#define smallIntegerValueOf(op) ((op & 0x80000000) ? (((-1 << 31) | op) >> 1) : (op >> 1))
///#define smallIntegerObjectOf(value) ((value << 1) | 1)
///#define fetchNamedSmallInteger(op, fieldIndex) (smallIntegerValueOf(fetchNamedPointer(op, fieldIndex)))
///#define storeNamedSmallInteger(op, fieldIndex, value) (storeNamedPointer(op, fieldIndex, smallIntegerObjectOf(value)))
///#define incrementNamedSmallInteger(op, fieldIndex) (addToNamedSmallInteger(op, fieldIndex, 1))
///#define decrementNamedSmallInteger(op, fieldIndex) (addToNamedSmallInteger(op, fieldIndex, -1))

// from interpreter.c
///OP activeContext;
///int activeContextIsOld;
///int activeContextPc;
///OP activeContextMethod;
///unsigned char * activeContextMethodBytes;
///OP activeContextOuterContext;
///OBJ activeContextTemporaries;
///#define fetchNextBytecode() ((int) (*(activeContextMethodBytes + activeContextPc)))
///#define incrementProgramCounter() (++activeContextPc)
///#define getActiveContext() (activeContext)
///#define jump(offset) (activeContextPc += offset)

// from interpreter-sends.c
///#define sendSelector(selector, receiver, argumentCount) (callMethod(receiver, selector, fetchClass(receiver), argumentCount))

// from interpreter-sends-lookup.c
///int methodLookupClassCacheEntries;
///OP * methodLookupSelectorCache;
///#define hashClassForLookupCache(class) (((class) % methodLookupClassCacheEntries) << 1)
///#define addMethodToLookupCache(selector, method, position) (methodLookupSelectorCache[position] = (selector), methodLookupSelectorCache[(position) + 1] = (method))

// from context-changing.c
///#define returnToParentContext() (returnToContext(fetchContextSender(getActiveContext())))
///#define returnValueToParentContext(returnValue) (returnValueToContext(fetchContextSender(getActiveContext()), returnValue))
///#define returnValueToContext(targetContext, returnValue) (switchActiveContext(targetContext), activeContextStackPush(returnValue))

// from context-stack.c
///OBJ activeContextStack;
///int activeContextSp;
///#define stackPeekDown(context, indexFromTop) (fetchIndexablePointer(context, fetchNamedSmallInteger(context, METHODCONTEXT_STACK_POINTER) - indexFromTop - 1))
///#define stackPeek(context) (stackPeekDown(context, 0))
///#define activeContextStackPush(value) (*(activeContextStack + activeContextSp++) = value, (activeContextIsOld && isNewObject(value)) ? (addToRemembered(activeContext), 1) : 0)
///#define activeContextStackPop() (*(activeContextStack + --activeContextSp))
///#define activeContextStackDecrement(indexFromTop) (activeContextSp -= indexFromTop)
///#define activeContextStackIncrement(indexFromTop) (activeContextSp += indexFromTop)
///#define activeContextStackPeek() (activeContextStackPeekDown(0))
///#define activeContextStackPeekDown(indexFromTop) (*(activeContextStack + activeContextSp - (indexFromTop) - 1))
///#define activeContextStackLoad() (activeContextSp = fetchNamedSmallInteger(getActiveContext(), METHODCONTEXT_STACK_POINTER))
///#define activeContextStackSave() (storeNamedPointer(getActiveContext(), METHODCONTEXT_STACK_POINTER, smallIntegerObjectOf(activeContextSp)))

// from context-helpers.c
///#define fetchContextMethod(context) (fetchNamedPointer(context, METHODCONTEXT_METHOD))
///#define fetchContextSender(context) (fetchNamedPointer(context, METHODCONTEXT_SENDER))
///#define fetchContextReceiver(context) (fetchNamedPointer(context, METHODCONTEXT_RECEIVER))
///#define fetchContextClosure(context) (fetchNamedPointer(context, METHODCONTEXT_CLOSURE))
///#define fetchContextOuterContext(context) (fetchNamedPointer(context, METHODCONTEXT_OUTER_CONTEXT))
///#define fetchClosureOuterContext(closure) (fetchNamedPointer(closure, BLOCKCLOSURE_OUTER_CONTEXT))
///#define fetchMethodLiteral(method, index) (fetchIndexablePointer(method, index))
///#define fetchContextTemporary(context, index) (fetchIndexablePointer(fetchContextOuterContext(context), index))
///#define storeContextTemporary(context, index, value) (storeIndexablePointer(fetchContextOuterContext(context), index, value))

#endif


void error(char * message) {
	printf(message);
	printf("\n");
	
	deallocateAll();
	exit(1);
}
