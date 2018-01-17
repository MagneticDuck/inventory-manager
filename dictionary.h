/**
 * Abstract dictionary type storing key-value pairs. Value agnostic, but specialized for string keys.
 */
#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

#include "util.h"

// All keys should have same length. Doing a lookup with a partial key will look for keys starting with that prefix.

typedef struct Dictionary Dictionary;
typedef struct DictionaryEntry DictEntry;

Dictionary * newDictionary();
void freeDictionary(Dictionary *);
size_t dictSize(Dictionary *);

DictEntry * dictLookup(Dictionary *, char * key);
DictEntry * dictAdd(Dictionary *, char * key, void * value); // Returns NULL if key is already in dictionary.
void dictRemove(Dictionary *, DictEntry *);

void * dictValue(DictEntry *);
char * dictKey(DictEntry *); // Do not edit!

#endif // DICTIONARY_H_INCLUDED
