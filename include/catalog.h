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

// Lifetime management and file IO.

ReadStatus newCatalogFromFile(Catalog ** catalog, Filepath filepath);
void newCatalogEmpty(Catalog ** catalog);
void newCatalogDemo(Catalog ** catalog);

void writeCatalog(Catalog * catalog, char * filepath);
void deleteCatalog(Catalog *);

// Accessing and modifying records.

ProductEntry * catGetProductByID(Catalog * catalog, ProductID * id);
ProductID * catGetProductID(ProductEntry * product);
ProductRecord * catGetRecord(ProductEntry * product);
void catDeleteEntry(ProductEntry *);

// Listings.

typedef struct {
    bool orderAlphabetical;
    bool useFilter;
    int categoryFilter;
} ListingConfig;

ProductEntry * catGetNext(Catalog *, ListingConfig * config, ProductEntry *);
ProductEntry * catSeekBy(Catalog *, ListingConfig * config, ProductEntry *, size_t seeking);
ProductEntry * catGetPrev(Catalog *, ListingConfig * config, ProductEntry *);

ProductEntry * catAlphabeticalSupremum(Catalog *, ListingConfig * config, char * prefix);
ProductEntry * catPriceSupremum(Catalog *, ListingConfig *config, int price);
ProductEntry * catLookup(Catalog *, ListingConfig *config, ProductID id);

// Miscellaneous stuff.

ReadStatus loadRecords(Catalog * catalog, FILE * input);

#endif // CATALOG_H_INCLUDED
