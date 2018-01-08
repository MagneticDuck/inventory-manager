#include "catalog.h"
#include "dictionary.h"
#include "ordered_list.h"
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
    unsigned int totalInstances;
} Catalog;

bool onDefCategory_(Category * category, void * ptr) {
    Catalog * catalog = (Catalog *) ptr;
    catalog->categories[catalog->categoryCount++] = category;
    olAdd(catalog->categoryLeaderboard, (void *) 0, (void *) category);
}

bool onDefRecord_(ProductID id, ProductRecord * record, void * ptr) {
    Catalog * catalog = (Catalog *) ptr;
    dictAdd(catalog->byId, (void *) id, (void *) record);
    olAdd(catalog->byName, (void *) record->name, (void *) record);
    olAdd(catalog->byPrice, (void *) record->price, (void *) record);
    ++catalog->totalRecords;
    catalog->totalInstances += record->instances;
}

ReadStatus newCatalogFromFile(Catalog ** catalog, Filepath filepath) {
    newCatalogEmpty(catalog);
    readFlatfile(filepath, (void *) *catalog, &onDefCategory_, &onDefRecord_);
}

void newCatalogEmpty(Catalog ** catalog) {
    *catalog = malloc(sizeof(Catalog));
    Catalog * catalog_ = *catalog;
    catalog_->byId = newDictionary();
    catalog_->byName = newDictionary(&lexiographicCompare);
    catalog_->byPrice = newDictionary(&numericCompare);
}

void newCatalogDemo(Catalog ** catalog) {
    newCatalogEmpty(catalog);
    readDemo((void *) *catalog, &onDefCategory_, &onDefRecord_);
}

typedef struct {
    Catalog * catalog;
    size_t categoryIndex;
    OLNode * head;
} CatalogIterator;

bool popCategory_(void * ptr, Category ** category) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(iterator->categoryIndex > iterator->catalog->categoryCount) return false;
    *category = iterator->catalog->categories[iterator->catalog->categoryCount++];
    return true;
}

bool popRecord_(void * ptr, ProductID ** id, ProductRecord ** record) {
    return false; // TODO
}

void writeCatalog(Catalog * catalog, char * filepath) {
    CatalogIterator iterator;
    iterator.catalog = catalog;
    iterator.head = olFirst(catalog->byName);
    iterator.categoryIndex = 0;
    writeFlatfile(filepath, (void *) &iterator, &popCategory_, &popRecord_);
}

void freeCatalog(Catalog * catalog) {
    freeDictionary(catalog->byId);
    freeOrderedList(catalog->byName);
    freeOrderedList(catalog->byPrice);
    freeOrderedList(catalog->categoryLeaderboard);
    for(size_t i = 0; i < catalog->categoryCount; ++i)
        free(catalog->categories[i]);
    free(catalog);
}

ProductEntry * catGetProductByID(Catalog * catalog, ProductID * id) {

}

ProductID * catGetProductID(ProductEntry * product) {
    return product->id;
}

ProductRecord * catGetRecord(ProductEntry * product) {

}

void catRemove(Catalog * catalog, ProductEntry * entry) {
    dictRemove(catalog->byId, entry->byId);
    olRemove(catalog->byName, entry->byName);
    olRemove(catalog->byPrice, entry->byPrice);
}

OLNode * getRelevantNode_(ProductEntry * entry, ListingConfig * config) {
    if(!config->categoryFilter) {
        if(config->orderAlphabetical) return entry->byName;
        else return entry->byPrice;
    } else {
        unimplemented();
        return NULL;
    }
}

OrderedList * getRelevantList_(Catalog * catalog, ListingConfig * config) {
    if(!config->categoryFilter) {
        if(config->orderAlphabetical) return catalog->byName;
        else return catalog->byPrice;
    } else {
        unimplemented();
        return NULL;
    }
}

ProductEntry * catFirst(Catalog * catalog, ListingConfig *config) {
    return (ProductEntry *) olValue(
        olFirst(getRelevantList_(catalog, config)));
}

ProductEntry * catLast(Catalog * catalog, ListingConfig *config) {
    return (ProductEntry *) olValue(
        olLast(getRelevantList_(catalog, config)));
}

ProductEntry * catNext(Catalog * catalog, ListingConfig * config, ProductEntry * entry) {
    return (ProductEntry *) olValue(
               olNext(getRelevantNode_(entry, config)));
}

ProductEntry * catSeekBy(Catalog * catalog, ListingConfig * config, ProductEntry * entry, size_t seeking) {
    return (ProductEntry *) olValue(
               olSeekBy(getRelevantNode_(entry, config), seeking));
}

ProductEntry * catPrev(Catalog * catalog, ListingConfig * config, ProductEntry * entry) {
    return (ProductEntry *) olValue(
               olPrev(getRelevantNode_(entry, config)));
}

ProductEntry * catAlphabeticalSupremum(Catalog * catalog, ListingConfig * config, char * prefix) {

}

ProductEntry * catPriceSupremum(Catalog * catalog, ListingConfig * config, int price) {

}

ProductEntry * catLookup(Catalog * catalog, ListingConfig * config, ProductID id) {
    return (ProductEntry *) dictValue(dictLookup(catalog->byId, id));
}

ReadStatus loadRecords(Catalog * catalog, FILE * input) {

}
