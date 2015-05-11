#ifndef __INTERPRETER_SENDS_LOOKUP_H__
#define __INTERPRETER_SENDS_LOOKUP_H__

OP lookupMethod(OP, OP);
void allocateMethodLookupCache();
void deallocateMethodLookupCache();
void enlargeMethodLookupCache();
void processMethodLookupCache();
void updateMethodLookupCache();
void removeSelectorFromMethodLookupCache(OP);
OP lookupMethodInCache(OP, OP);

#endif
