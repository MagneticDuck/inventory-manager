#include "tinytest.h"
#include "util.h"
#include "data.h"
#include "ordered_list.h"
#include "dictionary.h"

// These tests were written as a part of the code's development and now are essentially safeguards against regression.
// They are not meant to be complete unit-tests.

void testLexiographicCompare() {
    ASSERT("\"\" <= \"a\"", lexiographicCompare("", "a"));
    ASSERT("\"a\" <= \"a\"", lexiographicCompare("a", "a"));
    ASSERT("\"a\" <= \"ab\"", lexiographicCompare("a", "ab"));
    ASSERT("\"ab\" <= \"ac\"", lexiographicCompare("ab", "ac"));
    ASSERT("\"ab\" <= \"ba\"", lexiographicCompare("ab", "ba"));
}

void testOrderedList() {
    OrderedList * list = newOrderedList(&lexiographicCompare);
    OLNode * nodes[4];

    nodes[0] = olAdd(list, "abc", NULL);
    nodes[1] = olAdd(list, "", NULL);
    nodes[2] = olAdd(list, "abba", NULL);
    nodes[3] = olAdd(list, "abb", (void *) 42);

    ASSERT_EQUALS(olIndex(nodes[1]), 0);
    ASSERT_EQUALS(olIndex(nodes[3]), 1);
    ASSERT_EQUALS(olIndex(nodes[2]), 2);
    ASSERT_EQUALS(olIndex(nodes[0]), 3);

    ASSERT_EQUALS(olNext(nodes[1]), nodes[3]);
    ASSERT_EQUALS(olPrev(nodes[3]), nodes[1]);
    ASSERT_EQUALS(olSupremum(list, "hello world"), NULL);
    ASSERT_STRING_EQUALS(olKey(olSupremum(list, "ab")), "abb");
    ASSERT_STRING_EQUALS(olKey(olSupremum(list, "")), "");
    ASSERT_STRING_EQUALS(olKey(olSupremum(list, "abb")), "abb");

    olRemove(list, nodes[1]);
    ASSERT_EQUALS(olFirst(list), nodes[3]);
    ASSERT_EQUALS(olPrev(nodes[3]), NULL);

    freeOrderedList(list);
}

void testDictionary() {
    Dictionary * dictionary = newDictionary();
    DictEntry * entries[3];
    entries[0] = dictAdd(dictionary, "hello", "world");
    entries[1] = dictAdd(dictionary, "goodbye", "world");
    entries[2] = dictAdd(dictionary, "", "oops");

    ASSERT_STRING_EQUALS(*dictValue(entries[0]), "world");
    ASSERT_STRING_EQUALS(dictKey(entries[0]), "hello");

    ASSERT_EQUALS(dictLookup(dictionary, "hello"), entries[0]);
    ASSERT_EQUALS(dictLookup(dictionary, ""), entries[2]);
    ASSERT_EQUALS(dictLookup(dictionary, "oops"), NULL);

    ASSERT_EQUALS(dictAdd(dictionary, "hello", "not world"), NULL);
    dictRemove(dictionary, entries[0]);
    ASSERT_EQUALS(dictLookup(dictionary, "hello"), NULL);

    freeDictionary(dictionary);
}

void testRandom() {
    char word[10];
    randomWordFixed(9, word);
    Category * category = newRandomCategory(1);
    ProductRecord * record = newRandomRecord(category);

    printf("%s\n", word);
    ppRecord(record);
    ppCategory(category);

    // ASSERT_LOOKS_OK

    free(record);
    free(category);
}

int main() {
    initRandomSeed();
    RUN(testLexiographicCompare);
    RUN(testOrderedList);
    RUN(testDictionary);
    RUN(testRandom);
    return TEST_REPORT();
}
