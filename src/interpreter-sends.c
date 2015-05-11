#include "common.h"
#include "constants.h"
#include "interpreter-sends.h"
#include "interpreter-sends-lookup.h"
#include "interpreter.h"
#include "context-creation.h"
#include "context-helpers.h"
#include "context-stack.h"
#include "context-stack-active.h"
#include "vm.h"
#include "objectMemory.h"
#include "objectMemory-objects.h"
#include "objectMemory-smallIntegers.h"
#include "objectMemory-characters.h"
#include "objectMemory-gc.h"
#include "objectMemory-gc-old.h"


OP (* primitiveMethods[128])(OP, OP *);

OP callReceiver = OBJECT_NIL_OP;
OP callSelector = OBJECT_NIL_OP;
OP callLookupClass = OBJECT_NIL_OP;

int activeContextSp;


// optimization: replaced with macros
#ifndef OPTIMIZED_MACROS

void sendSelector(OP selector, OP receiver, int argumentCount) {
	OP lookupClass = fetchClass(receiver);
	
	callMethod(receiver, selector, lookupClass, argumentCount);
}

#endif

void sendSelectorToSuper(OP selector, OP receiver, int argumentCount) {
	OP senderContext = getActiveContext();
	OP senderMethod = fetchContextMethod(senderContext);
	OP senderMethodClass = fetchNamedPointer(senderMethod, COMPILEDMETHOD_CLASS);
	OP lookupClass = fetchNamedPointer(senderMethodClass, BEHAVIOR_SUPERCLASS);
	
	callMethod(receiver, selector, lookupClass, argumentCount);
}

// used by new space GC: interpreter registers act as roots of reachability in garbage collection
void processInterpreterCallRegisters() {
	if(processNewObject(callReceiver)) {
		callReceiver = fetchObjectPointer(callReceiver);
	}
	
	// selector and lookupClass are already in the old space,
	// no need to process them
}

// used by old space GC
void updateInterpreterCallRegisters() {
	updatePointer(&callReceiver);
	updatePointer(&callSelector);
	updatePointer(&callLookupClass);
}

void callMethod(OP receiver, OP selector, OP lookupClass, int argumentCount) {
	// at this point the caller has already placed the arguments and the receiver
	// on the stack: the receiver is at the bottom, followed by the arguments
	// (the first one is at the bottom, the last one is at the top)
	
	// ensure there is some new space left so that garbage collector will not be triggered
	// during the execution of this method and change OPs stored in local variables
	if(!hasNewSpaceAtLeast(1000)) {
		callReceiver = receiver;
		callSelector = selector;
		callLookupClass = lookupClass;
		
		do {
			garbageCollectNewSpace();
		} while(!hasNewSpaceAtLeast(1000));
		
		receiver = callReceiver;
		selector = callSelector;
		lookupClass = callLookupClass;
		
		callReceiver = OBJECT_NIL_OP;
		callSelector = OBJECT_NIL_OP;
		callLookupClass = OBJECT_NIL_OP;
	}
	
	OP method = lookupMethodInCache(selector, lookupClass);
	
	if(method == OBJECT_NIL_OP) {
		// sanity check: the object *has to* understand #doesNotUnderstand:
		// (it is implemented by ProtoObject); if it doesn't, something is wrong
		if(selector == OBJECT_DOES_NOT_UNDERSTAND_SELECTOR_OP) {
			error("Receiver does not understand #doesNotUnderstand: either.");
		}
		
		sendDoesNotUnderstandMessage(receiver, selector, lookupClass, argumentCount);
		return;
	}
	
	OP primitiveOp = fetchNamedPointer(method, COMPILEDMETHOD_PRIMITIVE);
	
	// if there is a primitive function available, execute it; if it fails,
	// execute the bytecode as if there were no primitive function available
	if(primitiveOp != OBJECT_NIL_OP) {
		OP senderContext = getActiveContext();
		int primitive = smallIntegerValueOf(primitiveOp);
		
		int stackPointer = activeContextSp;
		
		// do not copy the arguments elsewhere, take them directly from the stack
		OP * arguments;
		if(argumentCount) {
			arguments = objectFor(senderContext) + stackPointer - argumentCount + fetchFieldOf(senderContext, OBJECT_INDEXABLE_POINTER_FIRST_FIELD);
		}
		else {
			arguments = NULL;
		}
		
		activeContextStackDecrement(argumentCount + 1); // + 1 = the receiver
		
		OP result = (*primitiveMethods[primitive])(receiver, arguments);
		
		if(result != PRIMITIVE_FAIL_OP) {
			if(result != PRIMITIVE_VOID_OP) {
				// a little optimization to save one function call if the active context stays the same
				if(senderContext == getActiveContext()) {
					activeContextStackPush(result);
				}
				else {
					stackPush(senderContext, result);
				}
			}
			
			return;
		}
		else {
			// the primitive function has failed, "return" the arguments and the receiver to the stack
			if(senderContext == getActiveContext()) {
				activeContextStackIncrement(argumentCount + 1); // + 1 = the receiver
			}
			else {
				stackIncrement(senderContext, argumentCount + 1); // + 1 = the receiver
			}
		}
	}
	
	// there is no primitive available or the primitive has failed, create
	// a context for this method and execute its bytecodes
	OP context = createContextForMethod(method, receiver, getActiveContext(), selector);
	OP argument;
	
	// move the arguments to the new context (as parameters/temporaries);
	// they are popped in reverse order
	for(int index = argumentCount - 1; index >= 0; --index) {
		argument = activeContextStackPop();
		storeIndexablePointer(context, index, argument);
	}
	activeContextStackDecrement(1); // remove the receiver
	
	switchActiveContext(context);
}

void sendDoesNotUnderstandMessage(OP receiver, OP selector, OP lookupClass, int argumentCount) {
	// the argument of a #doesNotUnderstand: message is a Message object
	// which stores the original selector, arguments and the class where
	// method lookup started
	
	OP arguments = instantiateClass(ARRAY_OP, argumentCount, 0);
	OP argument;
	for(int index = argumentCount - 1; index >= 0; --index) {
		argument = activeContextStackPop();
		storeIndexablePointer(arguments, index, argument);
	}
	
	OP message = instantiateClass(MESSAGE_OP, 0, 0);
	storeNamedPointer(message, MESSAGE_SELECTOR, selector);
	storeNamedPointer(message, MESSAGE_ARGUMENTS, arguments);
	storeNamedPointer(message, MESSAGE_LOOKUP_CLASS, lookupClass);
	
	// there is always at least one available space on the stack, even if
	// the stack was full and the original message didn't have any arguments;
	// contexts are always created with one extra space for this exact situation
	activeContextStackPush(message);
	sendSelector(OBJECT_DOES_NOT_UNDERSTAND_SELECTOR_OP, receiver, 1);
}
