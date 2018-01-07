#include "tinytest.h"
#include "util.h"
#include "algorithms/ordered_list.h"
#include "algorithms/dictionary.h"

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

    nodes[0] = olAdd(list, "abc", (void *) 0);
    nodes[1] = olAdd(list, "", (void *) 1);
    nodes[2] = olAdd(list, "abba", (void *) 2);
    nodes[3] = olAdd(list, "abb", (void *) 3);

    ASSERT_EQUALS(olIndex(nodes[1]), 0);
    ASSERT_EQUALS(olIndex(nodes[3]), 1);
    ASSERT_EQUALS(olIndex(nodes[2]), 2);
    ASSERT_EQUALS(olIndex(nodes[0]), 3);

    ASSERT_EQUALS(olNext(nodes[1]), nodes[3]);
    ASSERT_EQUALS(olPrev(nodes[3]), nodes[1]);
    ASSERT_EQUALS(olSupremum(list, "hello world"), NULL);
    ASSERT_STRING_EQUALS(olKey(olSupremum(list, "ab")), "abb");
    ASSERT_STRING_EQUALS(olKey(olSupremum(list, "")), "");

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

    ASSERT_STRING_EQUALS(dictValue(entries[0]), "world");
    ASSERT_STRING_EQUALS(dictKey(entries[0]), "hello");

    ASSERT_EQUALS(dictLookup(dictionary, "hello"), entries[0]);
    ASSERT_EQUALS(dictLookup(dictionary, ""), entries[2]);
    ASSERT_EQUALS(dictLookup(dictionary, "oops"), NULL);

    freeDictionary(dictionary);
}

int main() {
    RUN(testLexiographicCompare);
    RUN(testOrderedList);
    RUN(testDictionary);
    return TEST_REPORT();
}
