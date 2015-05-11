#include <stdio.h> /* NULL */
#include "common.h"
#include "constants.h"
#include "interpreter-instructions.h"
#include "interpreter-instructions-closure.h"
#include "interpreter-instructions-jump.h"
#include "interpreter-instructions-push.h"
#include "interpreter-instructions-return.h"
#include "interpreter-instructions-send.h"
#include "interpreter-instructions-stack.h"
#include "interpreter-instructions-store.h"


// Bytecode/instruction list:
//  (X, Y, Z means the first, the second and the third argument, respectively)
// 
// byte   numArgs   instruction
// ----------------------------------------------------------------------------
// 1        1       push instance variable X
// 2        1       push class variable X
// 3        1       push temporary variable X
// 4        1       push global variable X
// 5        1       push literal constant X from constant pool
// 6        0       push self
// 7        0       push nil
// 8        0       push true
// 9        0       push false
// 10       0       push Smalltalk
// 11       0       push thisContext
// 12       1       store into instance variable X
// 13       1       store into class variable X
// 14       1       store into temporary variable X
// 16       0       pop
// 17       0       duplicate top
// 18       0       return top from method
// 19       0       return top from block
// 20       2       send selector X from constant pool with Y arguments
// 21       2       send selector X from constant pool with Y arguments to superclass
// 22       2       jump (X−128)×256+Y
// 23       2       pop and jump (X−128)×256+Y if true
// 24       2       pop and jump (X−128)×256+Y if false
// 25       2       pop and jump (X−128)×256+Y if nil
// 26       2       pop and jump (X−128)×256+Y if not nil
// 30       3       create block closure with X parameters, Y temporaries and variable offset Z
// 40       1       send special selector X with 0 arguments
// 41       1       send special selector X with 1 argument
// 42       1       send special selector X with 2 arguments
// 43       0       send selector #== with 1 argument
// 44       0       send selector #~~ with 1 argument
// 50       0       push instance variable 0
// 51       0       push instance variable 1
// 52       0       push instance variable 2
// 53       0       push instance variable 3
// 54       0       push instance variable 4
// 55       0       push temporary variable 0
// 56       0       push temporary variable 1
// 57       0       push temporary variable 2
// 58       0       push temporary variable 3
// 59       0       push temporary variable 4
// 60       0       return nil from method
// 61       0       return true from method
// 62       0       return false from method
// 63       0       return self from method
// 64       0       return instance variable 0 from method
// 65       0       return instance variable 1 from method
// 66       0       return instance variable 2 from method
// 67       0       return temporary variable 0 from method
// 68       0       return temporary variable 1 from method
// 69       0       return temporary variable 2 from method
// 70       0       push SmallInteger 1
// 71       0       push SmallInteger 0
// 72       0       push SmallInteger −1


void (* bytecodeInstructions[128])(OP) = {
	// 0
	NULL,
	interpretPushInstanceVariable,
	interpretPushClassVariable,
	interpretPushTemporaryVariable,
	interpretPushGlobalVariable,
	// 5
	interpretPushLiteralConstant,
	interpretPushSelf,
	interpretPushNil,
	interpretPushTrue,
	interpretPushFalse,
	// 10
	interpretPushSmalltalk,
	interpretPushThisContext,
	interpretStoreIntoInstanceVariable,
	interpretStoreIntoClassVariable,
	interpretStoreIntoTemporaryVariable,
	// 15
	NULL,
	interpretPop,
	interpretDuplicateTop,
	interpretReturnTopFromMethod,
	interpretReturnTopFromBlock,
	// 20
	interpretSendSelector,
	interpretSendSelectorToSuper,
	interpretJump,
	interpretPopJumpIfTrue,
	interpretPopJumpIfFalse,
	// 25
	interpretPopJumpIfNil,
	interpretPopJumpIfNotNil,
	NULL,
	NULL,
	NULL,
	// 30
	interpretCreateBlockClosure,
	NULL,
	NULL,
	NULL,
	NULL,
	// 35
	NULL, NULL, NULL, NULL, NULL,
	// 40
	interpretSendSpecialSelectorWith0Arguments,
	interpretSendSpecialSelectorWith1Argument,
	interpretSendSpecialSelectorWith2Arguments,
	interpretSendSelectorIdentical,
	interpretSendSelectorNotIdentical,
	// 45
	NULL, NULL, NULL, NULL, NULL,
	// 50
	interpretPushInstanceVariable0,
	interpretPushInstanceVariable1,
	interpretPushInstanceVariable2,
	interpretPushInstanceVariable3,
	interpretPushInstanceVariable4,
	// 55
	interpretPushTemporaryVariable0,
	interpretPushTemporaryVariable1,
	interpretPushTemporaryVariable2,
	interpretPushTemporaryVariable3,
	interpretPushTemporaryVariable4,
	// 60
	interpretReturnNilFromMethod,
	interpretReturnTrueFromMethod,
	interpretReturnFalseFromMethod,
	interpretReturnSelfFromMethod,
	interpretReturnInstanceVariable0FromMethod,
	// 65
	interpretReturnInstanceVariable1FromMethod,
	interpretReturnInstanceVariable2FromMethod,
	interpretReturnTemporaryVariable0FromMethod,
	interpretReturnTemporaryVariable1FromMethod,
	interpretReturnTemporaryVariable2FromMethod,
	// 70
	interpretPushSmallInteger1,
	interpretPushSmallInteger0,
	interpretPushSmallIntegerNeg1,
};
