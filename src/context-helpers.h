#ifndef __CONTEXT_HELPERS_H__
#define __CONTEXT_HELPERS_H__

OP fetchContextSender(OP);
OP fetchContextReceiver(OP);
OP fetchContextMethod(OP);
OP fetchContextClosure(OP);
OP fetchContextTemporary(OP, int);
OP storeContextTemporary(OP, int, OP);
OP fetchContextMethodLiteral(OP, int);
OP fetchClosureOuterContext(OP);

#endif
