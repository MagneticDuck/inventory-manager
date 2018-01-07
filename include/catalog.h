/**
 * catalog.h
 * Opaque top-level interface for the ProductCatalog, the composite data structure used to store, modify and query the product database.
 */
#ifndef CATALOG_H_INCLUDED
#define CATALOG_H_INCLUDED

#include <stdio.h>
#include "data.h"

typedef struct ProductEntry ProductEntry;
typedef struct ProductCatalog ProductCatalog;

// Lifetime management and file IO.

ReadStatus newCatalogFromFile(ProductCatalog ** catalog, Filepath filepath);
void newCatalog(ProductCatalog ** catalog);

void writeCatalog(ProductCatalog * catalog, char * filepath);
void deleteCatalog(ProductCatalog *);

// Accessing and modifying records.

ProductEntry * catGetProductByID(ProductCatalog * catalog, ProductID * id);
ProductID * catGetProductID(ProductEntry * product);
ProductRecord * catGetRecord(ProductEntry * product);
void catDeleteEntry(ProductEntry *);

// Listings.

typedef enum {
    bool orderAlphabetical;
    bool useFilter;
    Category filter;
} ListingConfig;

typedef struct {
    ProductEntry * (*getFirst)(void);
    ProductEntry * (*getLast)(void);
    ProductEntry * (*getPrevious)(ProductEntry *);
    ProductEntry * (*getLast)(ProductEntry *);
    size_t (*getSize)();
    size_t (*getIndex)(ProductEntry *);
    ProductEntry (*snapEntry)(ProductEntry *);
} ProductListing;

ProductListing * catGetListing(CatalogOrdering ordering, ListingConfig * filter);
ProductEntry * jumpByString(CatalogOrdering ordering, ListingConfig *);

// Miscellaneous stuff.

ReadStatus loadRecords(ProductCatalog * catalog, FILE * input);

#endif // CATALOG_H_INCLUDED
