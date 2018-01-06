#include "tinytest.h"
#include "util.h"
#include "algorithms/ordered_list.h"

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

    for (OLNode * i = olFirst(list); i; i = olNext(i)) {
        printf("key: %s\n", olKey(i));
    }

    ASSERT_EQUALS(olIndex(nodes[0]), 3);
    // more stuff here

}

int main() {
    RUN(testLexiographicCompare);
    RUN(testOrderedList);

    return TEST_REPORT();
}
