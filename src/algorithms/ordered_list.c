#include "util.h"
#include "algorithms/ordered_list.h"

#define SIMPLE_LIST
#ifdef SIMPLE_LIST
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
    while (list->first) olRemove(list->first);
    free(list);
}

OLNode * olFirst(OrderedList * list) {
    return list->first;
}

OLNode * olSupremum(OrderedList * list, void * key) {
    OLNode * current = list->first;
    loop {
        if (!current || (*list->ordering)(key, current->key)) return current;
        current = current->next;
    }
}

OLNode * olSeekBy(OrderedList * list, size_t seek) {
    OLNode * current = list->first;
    while (seek > 0 && current) {
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

void * olValue(OLNode * node) {
    return node->value;
}

void * olKey(OLNode * node) {
    return node->key;
}

size_t olIndex(OLNode * node) {
    size_t i = -1;
    for (OLNode * current = node; current; current = current->prev) ++i;
    return i;
}

OLNode * olAdd(OrderedList * list, void * key, void * value) {
    OLNode * successor = olSupremum(list, key),
             * newNode = malloc(sizeof(OLNode));
    newNode->key = key;
    newNode->value = value;

    if (!successor) {
        if (list->last) list->last->next = newNode;
        else list->first = newNode;
        list->first = newNode;
    } else {
        newNode->next = successor;
        newNode->prev = successor->prev;
        if (successor->prev) successor->prev->next = newNode;
        else list->first = newNode;
        successor->prev = newNode;
    }

    return newNode;
}

void olRemove(OLNode * node) {
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    free(node);
}

#else

// TODO: skiplist implementation

#endif
