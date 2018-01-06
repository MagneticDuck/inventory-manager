/**
 * Abstract dictionary type. Stores key-value pairs and permits fast searching by keys. A hashmap is used internally, relying on a supplied
 * hashing function to bin keys.
 */
#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED

typedef bool (*OrderingFunction)(void *, void *);

typedef struct Dictionary;
typedef struct DictionaryEntry;

Dictionary *newDictionary();
void freeDictionary(Dictionary *);

#endif // DICTIONARY_H_INCLUDED
