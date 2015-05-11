#ifndef __INTERPRETER_INSTRUCTIONS_SEND_H__
#define __INTERPRETER_INSTRUCTIONS_SEND_H__

void interpretSendSelector(OP);
void interpretSendSelectorToSuper(OP);
void interpretSendSpecialSelectorWith0Arguments(OP);
void interpretSendSpecialSelectorWith1Argument(OP);
void interpretSendSpecialSelectorWith2Arguments(OP);
void interpretSendSelectorIdentical(OP);
void interpretSendSelectorNotIdentical(OP);

#endif
