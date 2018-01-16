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

typedef struct ProductEntry {
    ProductRecord * record;
    DictEntry * byId;
    OLNode * byName[2];
    OLNode * byPrice[2];
} ProductEntry;

typedef struct Catalog Catalog;

// Lifetime management and file IO.

bool newCatalogFromFile(Catalog ** catalog, Filepath filepath);
void newCatalogRandom(Catalog ** catalog, size_t categories, size_t records);
void freeCatalog(Catalog *);

void writeCatalog(Catalog * catalog, char * filepath);

// Basic queries.

void ppRecord(Catalog *catalog, ProductEntry *entry);

size_t catRecordCount(Catalog *);
CategoryCode catCategoryCount(Catalog *);
char * catCategoryName(Catalog *, CategoryCode);
Price catCategoryValue(Catalog *, CategoryCode);
CategoryCode catCategoryByRank(Catalog *, size_t place);

// Accessing and modifying records.

ProductEntry * catLookupProduct(Catalog * catalog, ProductId id);
ProductRecord * catProductRecord(ProductEntry * product);
void catRemove(Catalog *, ProductEntry *);

// Listing the products.

typedef struct {
    bool orderAlphabetical;
    bool useFilter;
    int categoryFilter;
} ListingConfig;

ListingConfig * NULL_CONFIG(void);

ProductEntry * catFirst(Catalog *, ListingConfig *);
ProductEntry * catLast(Catalog *, ListingConfig *);
ProductEntry * catNext(Catalog *, ListingConfig *, ProductEntry *);
ProductEntry * catSeekBy(Catalog *, ListingConfig *, ProductEntry *, int seeking);
ProductEntry * catPrev(Catalog *, ListingConfig *, ProductEntry *);

ProductEntry * catAlphabeticalSupremum(Catalog *, ListingConfig * config, char * prefix);
ProductEntry * catPriceSupremum(Catalog *, ListingConfig * config, Price price);

#endif // CATALOG_H_INCLUDED
