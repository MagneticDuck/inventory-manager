/**
 * catalog.h
 * Opaque top-level interface for the ProductCatalog, the composite data structure used to store, modify and query the product database.
 */
#ifndef CATALOG_H_INCLUDED
#define CATALOG_H_INCLUDED

#include <stdio.h>
#include "data.h"
#include "util.h"

typedef struct ProductEntry ProductEntry;
typedef struct Catalog Catalog;

typedef struct {
    Price netPrice;
    size_t recordCount;
    size_t totalInstances;
} CategoryStats;

// Lifetime management and file IO.

bool newCatalogFromFile(Catalog ** catalog, Filepath filepath);
void newEmptyCatalog(Catalog ** catalog);
void newCatalogRandom(Catalog ** catalog, size_t categories, size_t records);
void freeCatalog(Catalog *);

void writeCatalog(Catalog * catalog, char * filepath);

// Basic queries.

size_t catTotalRecords(Catalog *);
size_t catTotalInstances(Catalog *);

CategoryCode catCategoryCount(Catalog *);
char * catCategoryName(Catalog *, CategoryCode);
CategoryStats * catCategoryStats(Catalog *, CategoryCode);
CategoryCode catCategoryByRank(Catalog *, size_t place);

// Accessing and modifying records.

ProductEntry * catLookupProduct(Catalog * catalog, ProductID * id);
ProductRecord * catProductRecord(ProductEntry * product);
void catRemove(Catalog *, ProductEntry *);

// Listing the products.

typedef struct {
    bool orderAlphabetical;
    bool useFilter;
    int categoryFilter;
} ListingConfig;

ProductEntry * catFirst(Catalog *, ListingConfig *);
ProductEntry * catLast(Catalog *, ListingConfig *);
ProductEntry * catNext(Catalog *, ListingConfig *, ProductEntry *);
ProductEntry * catSeekBy(Catalog *, ListingConfig *, ProductEntry *, size_t seeking);
ProductEntry * catPrev(Catalog *, ListingConfig *, ProductEntry *);

ProductEntry * catAlphabeticalSupremum(Catalog *, ListingConfig * config, char * prefix);
ProductEntry * catPriceSupremum(Catalog *, ListingConfig * config, int price);
ProductEntry * catLookup(Catalog *, ListingConfig * config, ProductID id);

#endif // CATALOG_H_INCLUDED
