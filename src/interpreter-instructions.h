#ifndef __INTERPRETER_INSTRUCTIONS_H__
#define __INTERPRETER_INSTRUCTIONS_H__

void interpretPushInstanceVariable(OP);
void interpretPushClassVariable(OP);
void interpretPushTemporaryVariable(OP);
void interpretPushLiteralConstant(OP);
void interpretPushSelf(OP);
void interpretPushNil(OP);
void interpretPushTrue(OP);
void interpretPushFalse(OP);
void interpretPushSmalltalk(OP);
void interpretPushThisContext(OP);
void interpretStoreIntoInstanceVariable(OP);
void interpretStoreIntoClassVariable(OP);
void interpretStoreIntoTemporaryVariable(OP);
void interpretPop(OP);
void interpretDuplicateTop(OP);
void interpretReturnTopFromMethod(OP);
void interpretReturnTopFromBlock(OP);
void interpretSendSelector(OP);
void interpretSendSelectorToSuper(OP);
void interpretJump(OP);
void interpretPopJumpIfTrue(OP);
void interpretPopJumpIfFalse(OP);
void interpretPopJumpIfNil(OP);
void interpretPopJumpIfNotNil(OP);

#endif
