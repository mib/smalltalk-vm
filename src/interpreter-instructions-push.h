#ifndef __INTERPRETER_INSTRUCTIONS_PUSH_H__
#define __INTERPRETER_INSTRUCTIONS_PUSH_H__

void interpretPushInstanceVariable(OP);
void interpretPushClassVariable(OP);
void interpretPushTemporaryVariable(OP);
void interpretPushGlobalVariable(OP);
void interpretPushLiteralConstant(OP);
void interpretPushSelf(OP);
void interpretPushNil(OP);
void interpretPushTrue(OP);
void interpretPushFalse(OP);
void interpretPushSmalltalk(OP);
void interpretPushThisContext(OP);
void interpretPushInstanceVariable0(OP);
void interpretPushInstanceVariable1(OP);
void interpretPushInstanceVariable2(OP);
void interpretPushInstanceVariable3(OP);
void interpretPushInstanceVariable4(OP);
void interpretPushTemporaryVariable0(OP);
void interpretPushTemporaryVariable1(OP);
void interpretPushTemporaryVariable2(OP);
void interpretPushTemporaryVariable3(OP);
void interpretPushTemporaryVariable4(OP);
void interpretPushSmallInteger1(OP);
void interpretPushSmallInteger0(OP);
void interpretPushSmallIntegerNeg1(OP);

#endif
