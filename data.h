/**
 * Data record types and parsing / file IO routines.
 */
#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED
#include <stdio.h>
#include "util.h"

typedef char ProductId[PRODUCT_ID_LENGTH + 1];
typedef char ProductName[MAX_STRING_LENGTH + 1];
typedef char CategoryName[MAX_STRING_LENGTH + 1];

typedef int Price;
#define FORMAT_PRICE "%i"
typedef int CategoryCode;

typedef struct {
    ProductId id;
    ProductName name;
    Price price;
    CategoryCode category;
    int instances;
} ProductRecord;

// These methods have the responsibility of allocating the memory for the category name and ProductRecord.
// We guarantee that onDefCategory is called no more than MAX_CATEGORIES times, and no call to onDefRecord proceeds
// a call to onDefCategory.
bool readCatalogFile(
    Filepath filepath, void * catcher,
    bool (*onDefCategory)(void *, char *),
    bool (*onDefRecord)(void *, ProductRecord *));
bool readRandomCatalog(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (*onDefCategory)(void *, char *),
    bool (*onDefRecord)(void *, ProductRecord *));
bool readRecordFile(
    Filepath filepath, void * catcher,
    bool (*onReadRecord)(void *, ProductRecord *));

bool writeFile(
    Filepath filepath, void * iterator,
    bool (popCategory)(void *, char **),
    bool (popRecord)(void *, ProductRecord **));

#endif // DATA_H_INCLUDED
