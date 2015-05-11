#include "common.h"
#include "constants.h"
#include "primitives-arithmetics.h"
#include "objectMemory-smallIntegers.h"


// SmallInteger>>#+
OP primitiveSmallInteger_Add_(OP receiver, OP * arguments) {
	// printf("PrimitiveAdd: OP=%d + OP=%d\n", receiver, arguments[0]);
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		// printf("PrimitiveAdd: %d + %d\n", first, second);
		
		int result = first + second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#-
OP primitiveSmallInteger_Subtract_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first - second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#*
OP primitiveSmallInteger_Times_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first * second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#//
OP primitiveSmallInteger_IntegerDivide_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first / second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#\\ (this parenthesis is here just because a line cannot end with a backslash)
OP primitiveSmallInteger_Modulo_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		int first = smallIntegerValueOf(receiver);
		int second = smallIntegerValueOf(arguments[0]);
		
		int result = first % second;
		
		if(isSmallIntegerValue(result)) {
			return smallIntegerObjectOf(result);
		}
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#=
OP primitiveSmallInteger_Equal_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return receiver == arguments[0] ? OBJECT_TRUE_OP : OBJECT_FALSE_OP;
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#~=
OP primitiveSmallInteger_NotEqual_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return receiver != arguments[0] ? OBJECT_TRUE_OP : OBJECT_FALSE_OP;
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#<
OP primitiveSmallInteger_LessThan_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) < smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#<=
OP primitiveSmallInteger_LessThanOrEqual_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) <= smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#>
OP primitiveSmallInteger_GreaterThan_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) > smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}

// SmallInteger>>#>=
OP primitiveSmallInteger_GreaterThanOrEqual_(OP receiver, OP * arguments) {
	if(isSmallIntegerObject(receiver) && isSmallIntegerObject(arguments[0])) {
		return smallIntegerValueOf(receiver) >= smallIntegerValueOf(arguments[0])
			? OBJECT_TRUE_OP
			: OBJECT_FALSE_OP;
	}
	
	// there should be another block for Floats and LargeIntegers
	
	return PRIMITIVE_FAIL_OP;
}
