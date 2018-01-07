#include "catalog.h"
#include "algorithms/dictionary.h"
#include "algorithms/leaderboard.h"
#include "algorithms/ordered_list.h"

typedef struct ProductCatalog {
    OrderedList * categories;
    Dictionary * byId;
    OrderedList * byName;
    OrderedList * byPrice;
    size_t totalRecords;
    size_t totalInstances;
} ProductCatalog;

Catalog * newCatalog_() {
    ProductCatalog * catalog = malloc(sizeof(Catalog));
    catalog->categories = newOrderedList();
]

ReadStatus newCatalogFromFile(ProductCatalog ** catalog, Filepath filepath) {

    loadFlatfile(filepath)
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

ProductListing catGetListing(CatalogOrdering ordering, ListingConfig * filter) {

}

ProductEntry jumpByString(CatalogOrdering ordering, ListingConfig *) {

}

ReadStatus loadRecords(ProductCatalog * catalog, FILE * input) {

}
