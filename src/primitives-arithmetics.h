#ifndef __PRIMITIVES_ARITHMETICS_H__
#define __PRIMITIVES_ARITHMETICS_H__

OP primitiveSmallInteger_Add_(OP, OP *);
OP primitiveSmallInteger_Subtract_(OP, OP *);
OP primitiveSmallInteger_Times_(OP, OP *);
OP primitiveSmallInteger_IntegerDivide_(OP, OP *);
OP primitiveSmallInteger_Modulo_(OP, OP *);
OP primitiveSmallInteger_Equal_(OP, OP *);
OP primitiveSmallInteger_NotEqual_(OP, OP *);
OP primitiveSmallInteger_LessThan_(OP, OP *);
OP primitiveSmallInteger_LessThanOrEqual_(OP, OP *);
OP primitiveSmallInteger_GreaterThan_(OP, OP *);
OP primitiveSmallInteger_GreaterThanOrEqual_(OP, OP *);

#endif
