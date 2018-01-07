#include "catalog.h"
#include "algorithms/dictionary.h"
#include "algorithms/leaderboard.h"
#include "algorithms/ordered_list.h"

typedef struct CategoryRecord {
    Category * category;
    Price totalValue;
    size_t totalRecords;
    size_t totalInstances;
} CategoryRecord;

typedef struct Catalog {
    OrderedList * categories;
    Dictionary * byId;
    OrderedList * byName;
    OrderedList * byPrice;
    size_t totalRecords;
    size_t totalInstances;
} Catalog;


ReadStatus newCatalogFromFile(ProductCatalog ** catalog, Filepath filepath) {

}

void newCatalog(ProductCatalog ** catalog) {

}

void writeCatalog(ProductCatalog * catalog, char * filepath) {

}

void deleteCatalog(ProductCatalog *) {

}

ProductEntry catGetProductByID(ProductCatalog * catalog, ProductID * id) {

}

ProductID catGetProductID(ProductEntry * product) {

}

ProductRecord catGetRecord(ProductEntry * product) {

}

void catDeleteEntry(ProductEntry *) {

}

ProductEntry struct6_ProductListing::(* getFirst)(void) {

}

ProductEntry struct6_ProductListing::(* getLast)(void) {

}

ProductEntry struct6_ProductListing::(* getPrevious)(ProductEntry *) {

}

ProductEntry struct6_ProductListing::(* getLast)(ProductEntry *) {

}

struct6_ProductListing:: getSize() {

}

struct6_ProductListing:: getIndex(ProductEntry *) {

}

struct6_ProductListing:: snapEntry(ProductEntry *) {

}

ProductListing catGetListing(CatalogOrdering ordering, ListingConfig * filter) {

}

ProductEntry jumpByString(CatalogOrdering ordering, ListingConfig *) {

}

ReadStatus loadRecords(ProductCatalog * catalog, FILE * input) {

}
