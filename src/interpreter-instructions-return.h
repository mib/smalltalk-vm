#ifndef __INTERPRETER_INSTRUCTIONS_RETURN_H__
#define __INTERPRETER_INSTRUCTIONS_RETURN_H__

void interpretReturnTopFromMethod(OP);
void interpretReturnTopFromBlock(OP);
void interpretReturnNilFromMethod(OP);
void interpretReturnTrueFromMethod(OP);
void interpretReturnFalseFromMethod(OP);
void interpretReturnSelfFromMethod(OP);
void interpretReturnInstanceVariable0FromMethod(OP);
void interpretReturnInstanceVariable1FromMethod(OP);
void interpretReturnInstanceVariable2FromMethod(OP);
void interpretReturnTemporaryVariable0FromMethod(OP);
void interpretReturnTemporaryVariable1FromMethod(OP);
void interpretReturnTemporaryVariable2FromMethod(OP);

#endif
