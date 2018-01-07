// This is a compile-time option, not a cpp guard.
#define SIMPLE_ORDERED_LIST
#ifdef SIMPLE_ORDERED_LIST

#include "util.h"
#include "algorithms/ordered_list.h"

// When SIMPLE_ORDERED_LIST is enabled, we just use a linked list.

struct OrderedList {
    OrderingFunction ordering;
    OLNode * first, * last;
};

struct OLNode {
    void * key, * value;
    OLNode * next, * prev;
};

OrderedList * newOrderedList(OrderingFunction ordering) {
    OrderedList * list = malloc(sizeof(OrderedList));
    list->ordering = ordering;
    list->first = NULL;
    list->last = NULL;
    return list;
}

void freeOrderedList(OrderedList * list) {
    while(list->first) olRemove(list, list->first);
    free(list);
}

OLNode * olFirst(OrderedList * list) {
    return list->first;
}

OLNode * olSupremum(OrderedList * list, void * key) {
    OLNode * current = list->first;
    loop {
        if(!current || (*list->ordering)(key, current->key)) return current;
        current = current->next;
    }
}

OLNode * olSeekBy(OrderedList * list, size_t seek) {
    OLNode * current = list->first;
    while(seek > 0 && current) {
        current = current->next;
        --seek;
    }
    return current;
}

OLNode * olNext(OLNode * node) {
    return node->next;
}

OLNode * olPrev(OLNode * node) {
    return node->prev;
}


size_t olIndex(OLNode * node) {
    size_t i = -1;
    for(OLNode * current = node; current; current = current->prev) ++i;
    return i;
}

// Implementation detail. Insert a new node into the list, given a (null?)
// successor.
void olInsertNode_(OrderedList * list, OLNode * successor, OLNode * newNode) {
    if (!successor) {
        if (list->last) list->last->next = newNode;
        else list->first = newNode;
        list->last = newNode;
    } else {
        newNode->next = successor;
        newNode->prev = successor->prev;
        if (successor->prev) successor->prev->next = newNode;
        else list->first = newNode;
        successor->prev = newNode;
    }
}

OLNode * olAdd(OrderedList * list, void * key, void * value) {
    OLNode * successor = olSupremum(list, key),
             * newNode = malloc(sizeof(OLNode));
    newNode->key = key;
    newNode->value = value;
    olInsertNode_(list, successor, newNode);
    return newNode;
}

OLNode * olAddWithoutDuplication(OrderedList * list, void * key, void * value) {
    OLNode * successor = olSupremum(list, key);
    if (successor && strcmp(successor->key, key) == 0) return NULL;
    OLNode * newNode = malloc(sizeof(OLNode));
    newNode->key = key;
    newNode->value = value;
    olInsertNode_(list, successor, newNode);
    return newNode;
}

void olRemove(OrderedList *list, OLNode * node) {
    if (node->prev) node->prev->next = node->next;
    else list->first = node->next;
    if (node->next) node->next->prev = node->prev;
    else list->last = node->prev;
    free(node);
}

void ** olValue(OLNode * node) {
    return &node->value;
}

void * olKey(OLNode * node) {
    return node->key;
}

#else

// When SIMPLE_ORDERED_LIST is disabled, we use a stochastically balanced tree-like data structure called a skiplist.

#endif
