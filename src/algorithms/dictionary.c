// This is a compile-time option, not a cpp guard.
#define SIMPLE_DICTIONARY
#ifdef SIMPLE_DICTIONARY

#include "algorithms/dictionary.h"
#include "algorithms/ordered_list.h"

// With SIMPLE_DICTIONARY enabled, we just wrap an `OrderedList`.

// We need to pull a `DictionaryEntry` from lookup in the `OrderedList`, so the
// user data has to be in this struct -- the user data pointer in `OLNode`
// points to a `DictionaryEntry`. On the other hand, we need to point to the
// `OLNode` from here so we can edit and find the key, which the `OrderedList`
// certainly needs access to. So, say hello to an unintentional pointer
// loop! node->value->node->value ...

typedef struct DictionaryEntry {
    OLNode * node;
    void * value; // User data.
} DictionaryEntry;

typedef struct Dictionary {
    OrderedList * list;
} Dictionary;

Dictionary * newDictionary() {
    Dictionary * dictionary = malloc(sizeof(Dictionary));
    dictionary->list = newOrderedList(&lexiographicCompare);
    return dictionary;
}

void freeDictionary(Dictionary * dictionary) {
    freeOrderedList(dictionary->list);
    free(dictionary);
}

DictionaryEntry * dictLookup(Dictionary * dictionary, char * key) {
    OLNode * closest = olSupremum(dictionary->list, key);
    printf("key is %s\n", (char *) olKey(closest));
    if (0 == strcmp((char *) olKey(closest), key)) return *olValue(closest);
    return NULL;
}

DictionaryEntry * dictAdd(Dictionary * dictionary, char * key, void * value) {
    OLNode * newNode = olAddWithoutDuplication(dictionary->list, key, NULL);
    if (!newNode) return NULL;
    DictionaryEntry * entry = malloc(sizeof(DictionaryEntry));
    *olValue(newNode) = entry;
    entry->value = value;
    entry->node = newNode;
    return entry;
}

void dictRemove(Dictionary * dictionary, DictionaryEntry * entry) {
    olRemove(dictionary->list, entry->node);
    free(entry);
}

char * dictKey(DictionaryEntry * entry) {
    return olKey(entry->node);
}

void ** dictValue(DictionaryEntry * entry) {
    return &entry->value;
}

#else

#define BIN_BITS 16
#include "util.h"

// When SIMPLE_DICTIONARY is disabled, we use a hashmap with 2^BIN_BITS bins.

#endif
