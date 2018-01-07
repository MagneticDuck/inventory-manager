/**
 * Abstract dictionary type storing key-value pairs. Value agnostic, but specialized for string keys.
 */
#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

typedef struct Dictionary Dictionary;
typedef struct DictionaryEntry DictEntry;

Dictionary * newDictionary();
void freeDictionary(Dictionary *);

DictEntry * dictLookup(Dictionary *, char * key);
DictEntry * dictAdd(Dictionary *, char * key, void * value); // Returns NULL if key is already in dictionary.
void dictRemove(Dictionary *, DictEntry *);

char * dictKey(DictEntry *);
void * dictValue(DictEntry *);

#endif // DICTIONARY_H_INCLUDED
