#ifndef __OBJECTMEMORY_SMALLINTEGERS_H__
#define __OBJECTMEMORY_SMALLINTEGERS_H__

int smallIntegerValueOf(OP);
OP smallIntegerObjectOf(int);
int isSmallIntegerObject(OP);
int isSmallIntegerValue(int);
int fetchNamedSmallInteger(OP, int);
void storeNamedSmallInteger(OP, int, int);
int incrementNamedSmallInteger(OP, int);
int decrementNamedSmallInteger(OP, int);
int addToNamedSmallInteger(OP, int, int);

#endif
