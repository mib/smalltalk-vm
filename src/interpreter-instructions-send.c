#include "common.h"
#include "constants.h"
#include "interpreter-instructions-send.h"
#include "interpreter-instructions.h"
#include "interpreter.h"
#include "interpreter-sends.h"
#include "context-stack-active.h"
#include "context-helpers.h"


// special, heavily used selectors compiled as a special instruction;
// with 0, 1 or 2 arguments

OP specialSelectors0[6] = {3492 << 2, 3507 << 2, 3520 << 2, 3538 << 2, 3552 << 2, 3567 << 2};
// #class
// #new
// #basicNew
// #size
// #value
// #initialize

OP specialSelectors1[17] = {3587 << 2, 3598 << 2, 3610 << 2, 3621 << 2, 3633 << 2, 3644 << 2, 3656 << 2, 3667 << 2, 3678 << 2, 3689 << 2, 3700 << 2, 3716 << 2, 3730 << 2, 3749 << 2, 3762 << 2, 3775 << 2, 3789 << 2};
// #=
// #~=
// #<
// #<=
// #>
// #>=
// #+
// #-
// #*
// #/
// #value:
// #new:
// #basicNew:
// #at:
// #do:
// #and:
// #or:

OP specialSelectors2[3] = {3802 << 2, 3824 << 2, 3841 << 2};
// #value:value:
// #at:put:
// #to:do:


void interpretSendSelector(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	int argumentCount = fetchNextBytecode();
	incrementProgramCounter();
	OP method = fetchContextMethod(context);
	OP selector = fetchMethodLiteral(method, index);
	OP receiver = activeContextStackPeekDown(argumentCount);
	
	sendSelector(selector, receiver, argumentCount);
}

void interpretSendSelectorToSuper(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	int argumentCount = fetchNextBytecode();
	incrementProgramCounter();
	OP method = fetchContextMethod(context);
	OP selector = fetchMethodLiteral(method, index);
	OP receiver = activeContextStackPeekDown(argumentCount);
	
	sendSelectorToSuper(selector, receiver, argumentCount);
}

void interpretSendSpecialSelectorWith0Arguments(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP selector = specialSelectors0[index];
	OP receiver = activeContextStackPeekDown(0);
	
	sendSelector(selector, receiver, 0);
}

void interpretSendSpecialSelectorWith1Argument(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP selector = specialSelectors1[index];
	OP receiver = activeContextStackPeekDown(1);
	
	sendSelector(selector, receiver, 1);
}

void interpretSendSpecialSelectorWith2Arguments(OP context) {
	int index = fetchNextBytecode();
	incrementProgramCounter();
	OP selector = specialSelectors2[index];
	OP receiver = activeContextStackPeekDown(2);
	
	sendSelector(selector, receiver, 2);
}

void interpretSendSelectorIdentical(OP context) {
	OP argument = activeContextStackPop();
	OP receiver = activeContextStackPop();
	
	activeContextStackPush(receiver == argument ? OBJECT_TRUE_OP : OBJECT_FALSE_OP);
}

void interpretSendSelectorNotIdentical(OP context) {
	OP argument = activeContextStackPop();
	OP receiver = activeContextStackPop();
	
	activeContextStackPush(receiver == argument ? OBJECT_FALSE_OP : OBJECT_TRUE_OP);
}
