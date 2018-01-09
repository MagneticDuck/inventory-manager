#include "catalog.h"
#include "dictionary.h"
#include "ordered_list.h"
#include "data.h"

ListingConfig * NULL_CONFIG(void) {
    static ListingConfig config; // static!
    config.useFilter = false;
    config.orderAlphabetical = true;
    config.categoryFilter = 0;
    return &config;
}

typedef struct ProductEntry {
    ProductRecord * record;
    DictEntry * byId;
    OLNode * byName[MAX_CATEGORIES + 1];
    OLNode * byPrice[MAX_CATEGORIES + 1];
} ProductEntry;

typedef struct Catalog {
    Category * categories[MAX_CATEGORIES];
    CategoryStats categoryStats[MAX_CATEGORIES];
    size_t categoryCount;

    OrderedList * categoryLeaderboard;
    Dictionary * byId;
    OrderedList * byName[MAX_CATEGORIES + 1];
    OrderedList * byPrice[MAX_CATEGORIES + 1];

    size_t totalRecords;
    unsigned int totalInstances;
} Catalog;

void initCategoryStats_(CategoryStats * stats) {
    stats->netPrice = 0;
    stats->recordCount = 0;
    stats->totalInstance s= 0;
}

bool onDefCategory_(void * ptr, Category * category) {
    Catalog * catalog = (Catalog *) ptr;
    catalog->categories[catalog->categoryCount] = category;
    initCategoryStats_(&catalog->categoryStats[catalog->categoryCount]);
    olAdd(catalog->categoryLeaderboard, (void *) 0, (void *) category);

    catalog->byName[catalog->categoryCount] = newOrderedList(&lexiographicCompare);
    catalog->byPrice[catalog->categoryCount] = newOrderedList(&numericCompare);
    ++catalog->categoryCount;
    return true;
}

bool onDefRecord_(void * ptr, ProductRecord * record) {
    Catalog * catalog = (Catalog *) ptr;
    dictAdd(catalog->byId, (void *) record->id, (void *) record);
    for(size_t i = 0; i <= catalog->categoryCount; ++i) {
        olAdd(catalog->byName[i], (void *) record->name, (void *) record);
        olAdd(catalog->byPrice[i], (void *) record->price, (void *) record);
    }
    ++catalog->totalRecords;
    catalog->totalInstances += record->instances;
    return true;
}

void newEmptyCatalog(Catalog ** catalog) {
    *catalog = malloc(sizeof(Catalog));
    Catalog * catalog_ = *catalog;
    catalog_->byId = newDictionary();
    catalog_->categoryLeaderboard = newOrderedList(&numericCompare);
    catalog_->byName[0] = newOrderedList(&lexiographicCompare);
    catalog_->byPrice[0] = newOrderedList(&numericCompare);
}

bool newCatalogFromFile(Catalog ** catalog, Filepath filepath) {
    newEmptyCatalog(catalog);
    readCatalogFile(filepath, (void *) *catalog, &onDefCategory_, &onDefRecord_);
}

void newCatalogRandom(Catalog ** catalog, size_t categoryCount, size_t recordCount) {
    newEmptyCatalog(catalog);
    readRandomCatalog(categoryCount, recordCount, (void *) *catalog, &onDefCategory_, &onDefRecord_);
}

void freeCatalog(Catalog * catalog) {
    freeDictionary(catalog->byId);
    for(size_t i = 0; i <= catalog->categoryCount; ++i) {

        freeOrderedList(catalog->byName[i]);
        freeOrderedList(catalog->byPrice[i]);
    }
    freeOrderedList(catalog->categoryLeaderboard);
    for(size_t i = 0; i < catalog->categoryCount; ++i)
        free(catalog->categories[i]);
    free(catalog);
}

typedef struct {
    Catalog * catalog;
    size_t categoryIndex;
    ProductEntry * head;
} CatalogIterator;

bool popCategory_(void * ptr, Category ** category) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(iterator->categoryIndex > iterator->catalog->categoryCount) return false;
    *category = iterator->catalog->categories[iterator->catalog->categoryCount++];
    return true;
}

bool popRecord_(void * ptr, ProductRecord ** record) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(!iterator->head) return false;
    *record = catProductRecord(iterator->head);
    iterator->head = catNext(iterator->catalog, &NULL_CONFIG, iterator->head);
    return true;
}

void writeCatalog(Catalog * catalog, char * filepath) {
    CatalogIterator iterator;
    iterator.catalog = catalog;
    iterator.head = catFirst(catalog, NULL_CONFIG());
    iterator.categoryIndex = 0;
    writeFile(filepath, (void *) &iterator, &popCategory_, &popRecord_);
}

ProductEntry * catLookupProduct(Catalog * catalog, ProductID * id) {

}

ProductRecord * catProductRecord(ProductEntry * product) {
    return product->record;
}

void catRemove(Catalog * catalog, ProductEntry * entry) {
    dictRemove(catalog->byId, entry->byId);
    for(size_t i = 0; i <= catalog->categoryCount; ++i) {
        olRemove(catalog->byName[i], (entry->byName));
        olRemove(catalog->byPrice[i], entry->byPrice);
    }
}

OLNode * getRelevantNode_(ProductEntry * entry, ListingConfig * config) {
    OLNode ** index;
    if(config->orderAlphabetical) index = entry->byName;
    else index = entry->byPrice;
    if(!config->useFilter) return index[0];
    else return index[config->categoryFilter + 1];
}

OrderedList * getRelevantList_(Catalog * catalog, ListingConfig * config) {
    OrderedList ** index;
    if(config->orderAlphabetical) index = catalog->byName;
    else index = catalog->byPrice;
    if(!config->useFilter) return index[0];
    else return index[config->categoryFilter + 1];
}

ProductEntry * catFirst(Catalog * catalog, ListingConfig * config) {
    return (ProductEntry *) olValue(olFirst(getRelevantList_(catalog, config)));
}

ProductEntry * catLast(Catalog * catalog, ListingConfig * config) {
    return (ProductEntry *) olValue(olLast(getRelevantList_(catalog, config)));
}

ProductEntry * catNext(Catalog * catalog, ListingConfig * config, ProductEntry * entry) {
    return (ProductEntry *) olValue(olNext(getRelevantNode_(entry, config)));
}

ProductEntry * catSeekBy(Catalog * catalog, ListingConfig * config, ProductEntry * entry, size_t seeking) {
    return (ProductEntry *) olValue(olSeekBy(getRelevantNode_(entry, config), seeking));
}

ProductEntry * catPrev(Catalog * catalog, ListingConfig * config, ProductEntry * entry) {
    return (ProductEntry *) olValue(olPrev(getRelevantNode_(entry, config)));
}

ProductEntry * catAlphabeticalSupremum(Catalog * catalog, ListingConfig * config, char * prefix) {
    OrderedList * relevant;
    if(!config->useFilter) relevant = catalog->byName[0];
    else relevant = catalog->byName[config->categoryFilter + 1];
    return (ProductEntry *) olValue(olSupremum(relevant, prefix));
}

ProductEntry * catPriceSupremum(Catalog * catalog, ListingConfig * config, int price) {
    OrderedList * relevant;
    if(!config->useFilter) relevant = catalog->byPrice[0];
    else relevant = catalog->byPrice[config->categoryFilter + 1];
    return (ProductEntry *) olValue(olSupremum(relevant, price));
}

ProductEntry * catLookup(Catalog * catalog, ListingConfig * config, ProductID id) {
    return (ProductEntry *) dictValue(dictLookup(catalog->byId, id));
}
