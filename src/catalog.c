#include "catalog.h"
#include "algorithms/dictionary.h"
#include "algorithms/leaderboard.h"
#include "algorithms/ordered_list.h"
#include "data.h"

typedef struct ProductEntry {
    ProductRecord * record;
    ProductID * id;
    OLNode * byName;
    OLNode * byPrice;
    DictEntry * byId;
} ProductEntry;

typedef struct Catalog {
    Category ** categories;
    size_t categoryCount;

    OrderedList * categoryLeaderboard;
    Dictionary * byId;
    OrderedList * byName;
    OrderedList * byPrice;
    size_t totalRecords;
    size_t totalInstances;
} Catalog;

bool onDefCategory_(Category * category, void * catcher) {
    Catalog * catalog = (Catalog *) catcher;
    catalog->categories[catalog->categoryCount++] = category;
    olAdd(catalog->categoryLeaderboard, (void *) 0, (void *) category);
}

bool onDefRecord_(ProductRecord *, void * catcher) {
    Catalog * catalog = (Catalog *) catcher;
}

ReadStatus newCatalogFromFile(Catalog ** catalog, Filepath filepath) {
    newCatalogEmpty(catalog);

}

void newCatalogEmpty(Catalog ** catalog) {

}

void newCatalogDemo(Catalog ** catalog) {
    newCatalogEmpty(catalog);

}

void writeCatalog(Catalog * catalog, char * filepath) {

}

void deleteCatalog(Catalog *) {

}

ProductEntry * catGetProductByID(Catalog * catalog, ProductID * id) {

}

ProductID * catGetProductID(ProductEntry * product) {
    return product->id;
}

ProductRecord * catGetRecord(ProductEntry * product) {

}

void catDeleteEntry(ProductEntry *) {

}

ProductEntry catGetNext(Catalog *, ListingConfig * config, ProductEntry *) {

}

ProductEntry catSeekBy(Catalog *, ListingConfig * config, ProductEntry *, size_t seeking) {

}

ProductEntry catGetPrev(Catalog *, ListingConfig * config, ProductEntry *) {

}

ProductEntry catAlphabeticalSupremum(Catalog *, ListingConfig * config, char * prefix) {

}

ProductEntry catPriceSupremum(Catalog *, ListingConfig * config, int price) {

}

ProductEntry catLookup(Catalog *, ListingConfig * config, ProductID id) {

}

ReadStatus loadRecords(Catalog * catalog, FILE * input) {

}
