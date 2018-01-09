#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include "util.h"

typedef struct Array Array;

Array * newArray(size_t maxSize);
void freeArray(Array *);

size_t arraySize(Array *);
size_t arrayAppend(Array *, void * data);
void * arrayAccess(Array *, size_t index);

#endif // ARRAY_H_INCLUDED
