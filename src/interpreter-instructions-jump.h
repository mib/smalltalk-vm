#ifndef __INTERPRETER_INSTRUCTIONS_JUMP_H__
#define __INTERPRETER_INSTRUCTIONS_JUMP_H__

void interpretJump(OP);
void interpretPopJumpIfTrue(OP);
void interpretPopJumpIfFalse(OP);
void interpretPopJumpIfNil(OP);
void interpretPopJumpIfNotNil(OP);

#endif
