/**
 * catalog.h
 * Opaque top-level interface for the ProductCatalog, the composite data structure used to store, modify and query the product database.
 */
#ifndef CATALOG_H_INCLUDED
#define CATALOG_H_INCLUDED

#include <stdio.h>
#include "data.h"
#include "util.h"
#include "dictionary.h"
#include "ordered_list.h"

typedef struct ProductEntry ProductEntry; 
typedef struct Catalog Catalog;

// Lifetime management and file IO.

bool newCatalogFromFile(Catalog ** catalog, Filepath filepath);
void newCatalogRandom(Catalog ** catalog, size_t categories, size_t records);
void freeCatalog(Catalog *);

void writeCatalog(Catalog * catalog, char * filepath);

// Basic queries.

void ppRecord(char * string, Catalog * catalog, ProductEntry * entry);

CategoryCode catCategoryCount(Catalog *);
char * catCategoryName(Catalog *, CategoryCode);
Price catTotalValue(Catalog *);
Price catCategoryValue(Catalog *, CategoryCode);
CategoryCode catCategoryByRank(Catalog *, size_t place);

// Accessing and modifying records.
// Only the name, price and instances fields of ProductRecord may be changed.

ProductEntry * catLookupProduct(Catalog * catalog, ProductId id);
ProductRecord * catProductRecord(ProductEntry * product);
ProductEntry * catAddRecord(Catalog * catalog, ProductRecord * record); // copies data
void catRegisterRecordEdits(Catalog * catalog, ProductEntry * entry); // register edits made to the record
void catUndoRecordEdits(ProductEntry * entry);
void catRemove(Catalog *, ProductEntry *);

// Listing the products.

typedef struct {
    bool orderAlphabetical;
    bool useFilter;
    int categoryFilter;
} ListingConfig;

ListingConfig * NULL_CONFIG(void);

size_t catRecordCount(Catalog *, ListingConfig * config);
size_t catIndex(ListingConfig *, ProductEntry *);
ProductEntry * catFirst(Catalog *, ListingConfig *);
ProductEntry * catLast(Catalog *, ListingConfig *);
ProductEntry * catNext(ListingConfig *, ProductEntry *);
ProductEntry * catPrev(ListingConfig *, ProductEntry *);
ProductEntry * catSeekBy(ListingConfig *, ProductEntry *, int seeking);

ProductEntry * catAlphabeticalSupremum(Catalog *, ListingConfig * config, char * prefix);
ProductEntry * catPriceSupremum(Catalog *, ListingConfig * config, Price price);

#endif // CATALOG_H_INCLUDED
