/**
 * Abstract ordered data type containing a list of key-value pairs. The sequence of keys is ordered with respect to an ordering function.
 * An underlying skiplist data structure allows for fast seeking and edit operations.
 */
#ifndef ORDERED_LIST_H_INCLUDED
#define ORDERED_LIST_H_INCLUDED

#include "util.h"

typedef bool (* OrderingFunction)(void *, void *);

typedef struct OrderedList OrderedList;
typedef struct OLNode OLNode;

OrderedList * newOrderedList(OrderingFunction ordering);
void freeOrderedList(OrderedList *);

OLNode * olFirst(OrderedList *);
OLNode * olSupremum(OrderedList *, void * key); // Gives last node with key >= to the key presented.
OLNode * olSeekBy(OrderedList *, size_t seeking);
OLNode * olNext(OLNode *);
OLNode * olPrev(OLNode *);

void ** olValue(OLNode *);
void * olKey(OLNode *); // Do not edit!
size_t olIndex(OLNode *);

OLNode * olAdd(OrderedList *, void * key, void * value);
OLNode * olAddWithoutDuplication(OrderedList *, void * key, void * value); // Refuse to add if the key is already used.
void olReindex(OrderedList *, OLNode *, void * newKey);
void olRemove(OrderedList *, OLNode *);

#endif // ORDERED_LIST_H_INCLUDED
