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
#define FORMAT_PRICE "%i"
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
// We guarantee that onDefCategory is called no more than MAX_CATEGORIES times, and no call to onDefRecord proceeds
// a call to onDefCategory.
bool readCatalogFile(
    Filepath filepath, void * catcher,
    bool (*onDefCategory)(void *, Category *),
    bool (*onDefRecord)(void *, ProductRecord *));
bool readRandomCatalog(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (*onDefCategory)(void *, Category *),
    bool (*onDefRecord)(void *, ProductRecord *));
bool readRecordFile(
    Filepath filepath, void * catcher,
    bool (*onReadRecord)(void *, ProductRecord *));

bool writeFile(
    Filepath filepath, void * iterator,
    bool (popCategory)(void *, Category **),
    bool (popRecord)(void *, ProductRecord **));

#endif // DATA_H_INCLUDED
