/**
 * Data record types and parsing / file IO routines.
 */
#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED
#include <stdio.h>
#include "util.h"

#define PRODUCT_ID_LENGTH 20
#define MAX_STRING_LENGTH 50
#define MAX_CATEGORIES 100

typedef char ProductID[PRODUCT_ID_LENGTH];

typedef unsigned long int Price;
typedef unsigned char CategoryCode;

typedef struct {
    CategoryCode code;
    char name[MAX_STRING_LENGTH];
} Category;

void ppCategory(Category *);
void randomCategory(Category *, CategoryCode code);

typedef struct {
    char id[PRODUCT_ID_LENGTH];
    char name[MAX_STRING_LENGTH];
    Price price;
    Category *category;
    unsigned int instances;
} ProductRecord;

void ppRecord(ProductRecord *);
void randomRecord(ProductRecord *, Category *);

// These methods have the responsibility of allocating the memory for Category and ProductRecord.
// Of course, the responsibility of cleaning up falls on Catalog.

// True is for success, as usual.
bool loadFlatfile(
    Filepath filepath, void * reader,
    bool (*onDefCategory)(void *, Category *),
    bool (*onDefRecord)(void *, ProductRecord *));

bool loadRandom(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (*onDefCategory)(void *, Category *),
    bool (*onDefRecord)(void *, ProductRecord *));

bool writeFlatfile(
    Filepath filepath, void * iterator,
    bool (popCategory)(void *, Category **),
    bool (popRecord)(void *, ProductRecord **));

#endif // DATA_H_INCLUDED
