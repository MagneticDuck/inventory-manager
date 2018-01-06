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

ReadStatus loadCatalog(ProductCatalog * catalog, Filepath filepath);
ProductCatalog * demoCatalog();

void writeCatalog(ProductCatalog * catalog, char * filepath);
void deleteCatalog(ProductCatalog *);

typedef struct {
    bool isEffective;
    Category category;
} CategoryFilter;

ProductEntry * catGetProductByID(ProductCatalog * catalog, ProductID * id);
ProductEntry * catGetProductByPrefix(ProductCatalog * catalog, char * prefix, CategoryFilter * filter);
ProductEntry * catGetProductByIndex(ProductCatalog * catalog, char * prefix, CategoryFilter * filter);

ProductEntry * catNextProductAlphabetical(ProductEntry * pointer, CategoryFilter * filter);
ProductEntry * catPreviousProductAlphabetical(ProductEntry * pointer, CategoryFilter * filter);

void catMostValuableProducts(ProductCatalog * catalog, ProductEntry  * valuable[3]);
Price catNetValue(ProductCatalog * catalog);

void deleteEntry(ProductEntry *);
ProductRecord * getProductRecord(ProductEntry  * product);
void setProductRecord(ProductEntry *product, ProductRecord * record);
ProductID * getProductID(ProductEntry  * product);
size_t getProductIndex(ProductEntry  * product);

ReadStatus reconcileFile(FILE * file, ProductCatalog * catalog);

#endif // CATALOG_H_INCLUDED
