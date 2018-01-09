#include "array.h"
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

typedef struct {
    char * name;
    CategoryCode code;
    OLNode * byValue;
    Price netValue;
} CategoryEntry;

void freeCategoryEntry_(CategoryEntry * entry) {
    free(entry->name);
    free(entry);
}

typedef struct ProductEntry {
    ProductRecord * record;
    DictEntry * byId;
    OLNode * byName[MAX_CATEGORIES + 1];
    OLNode * byPrice[MAX_CATEGORIES + 1];
} ProductEntry;

typedef struct Catalog {
    // Index CategoryEntry.
    Array * categoriesByCode;
    OrderedList * categoriesByValue; // NULL unless requested

    // Index ProductEntry.
    Dictionary * productsById;
    OrderedList * productsByName[MAX_CATEGORIES + 1];
    OrderedList * productsByPrice[MAX_CATEGORIES + 1];
} Catalog;

CategoryEntry * catCategoryEntry_(Catalog * catalog, CategoryCode code) {
    return (CategoryEntry *) arrayAccess(catalog->categoriesByCode, code);
}

void catSortCategories_(Catalog * catalog) {
    if (catalog->categoriesByValue) freeOrderedList(catalog->categoriesByValue);
    // TODO
}

void catUnsortCategories_(Catalog * catalog) {
    if (catalog->categoriesByValue) freeOrderedList(catalog->categoriesByValue);
    catalog->categoriesByValue = NULL;
}

bool onReadCategory_(void * ptr, char * name) {
    Catalog * catalog = (Catalog *) ptr;

    CategoryEntry * entry = malloc(sizeof(CategoryEntry));
    entry->name = name;
    entry->code = arrayAppend(catalog->categoriesByCode, entry);
    entry->netValue = 0;
    olAdd(catalog->categoriesByValue, (void *) &entry->netValue, (void *) entry);

    catalog->productsByName[catCategoryCount(catalog) + 1] = newOrderedList(&lexiographicCompare);
    catalog->productsByPrice[catCategoryCount(catalog) + 1] = newOrderedList(&priceCompare);
    return true;
}

bool onReadRecord_(void * ptr, ProductRecord * record) {
    Catalog * catalog = (Catalog *) ptr;

    if (record->category > catCategoryCount(catalog)) {
        printf("Category code out of bounds!\n");
        return false;
    }

    // Add record to all the structures.
    dictAdd(catalog->productsById, (void *) record->id, (void *) record);
    olAdd(catalog->productsByName[0], (void *) record->name, (void *) record);
    olAdd(catalog->productsByPrice[0], (void *) &record->price, (void *) record);
    olAdd(catalog->productsByName[record->category], (void *) record->name, (void *) record);
    olAdd(catalog->productsByPrice[record->category], (void *) &record->price, (void *) record);

    // Update category net value and reindex in list.
    CategoryEntry * entry = (CategoryEntry *) arrayAccess(catalog->categoriesByCode, record->category);
    entry->netValue += record->price * record->instances;
    olReindex(catalog->categoriesByValue, &entry->netValue, entry);
    return true;
}

void newEmptyCatalog(Catalog ** catalog) {
    *catalog = malloc(sizeof(Catalog));
    Catalog * catalog_ = *catalog;

    catalog_->categoriesByCode = newArray(MAX_CATEGORIES);
    catalog_->categoriesByValue = newOrderedList(&priceCompare);

    catalog_->productsById = newDictionary();
    catalog_->productsByName[0] = newOrderedList(&lexiographicCompare);
    catalog_->productsByPrice[0] = newOrderedList(&priceCompare);
}

bool newCatalogFromFile(Catalog ** catalog, Filepath filepath) {
    newEmptyCatalog(catalog);
    return readCatalogFile(filepath, (void *) *catalog, &onReadCategory_, &onReadRecord_);
}

void newCatalogRandom(Catalog ** catalog, size_t categoryCount, size_t recordCount) {
    newEmptyCatalog(catalog);
    readRandomCatalog(categoryCount, recordCount, (void *) *catalog, &onReadCategory_, &onReadRecord_);
}

void freeCatalog(Catalog * catalog) {
    freeDictionary(catalog->productsById);
    for(size_t i = 0; i <= catCategoryCount(catalog); ++i) {
        freeOrderedList(catalog->productsByName[i]);
        freeOrderedList(catalog->productsByPrice[i]);
    }
    if (catalog->categoriesByValue) freeOrderedList(catalog->categoriesByValue);
    for(size_t i = 0; i < catCategoryCount(catalog); ++i)
        freeCategoryEntry_(arrayAccess(catalog->categoriesByCode, i));
    freeArray(catalog->categoriesByCode);
    free(catalog);
}

typedef struct {
    Catalog * catalog;
    size_t categoryIndex;
    ProductEntry * head;
} CatalogIterator;

bool popCategory_(void * ptr, char ** name) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(iterator->categoryIndex >= catCategoryCount(iterator->catalog)) return false;
    name = catCategoryEntry_(iterator->catalog, iterator->categoryIndex)->name;
    return true;
}

bool popRecord_(void * ptr, ProductRecord ** record) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(!iterator->head) return false;
    *record = catProductRecord(iterator->head);
    iterator->head = catNext(iterator->catalog, NULL_CONFIG(), iterator->head);
    return true;
}

void writeCatalog(Catalog * catalog, char * filepath) {
    CatalogIterator iterator;
    iterator.catalog = catalog;
    iterator.head = catFirst(catalog, NULL_CONFIG());
    iterator.categoryIndex = 0;
    writeFile(filepath, (void *) &iterator, &popCategory_, &popRecord_);
}

size_t catRecordCount(Catalog *catalog) {
    return dictSize(catalog->productsById);
}

CategoryCode catCategoryCount(Catalog *catalog) {
    return arraySize(catalog->categoriesByCode);
}

char * catCategoryName(Catalog *catalog, CategoryCode code) {
    return catCategoryEntry_(catalog, code)->name;
}

Price catCategoryValue(Catalog *catalog, CategoryCode code) {
    return catCategoryEntry_(catalog, code)->netValue;
}

CategoryCode catCategoryByRank(Catalog *catalog, size_t place) {
    catSortCategories_(catalog);
    OLNode * seeked = olSeekBy(olFirst(catalog->categoriesByValue), place);
    return ((CategoryEntry *) olValue(seeked))->code;
}

ProductEntry * catLookupProduct(Catalog * catalog, ProductId id) {
    return (ProductEntry *) dictValue(dictLookup(catalog->productsById, id));
}

ProductRecord * catProductRecord(ProductEntry * product) {
    return product->record;
}

void catRemove(Catalog * catalog, ProductEntry * entry) {
    dictRemove(catalog->productsById, entry->byId);
    for(size_t i = 0; i <= catCategoryCount(catalog); ++i) {
        olRemove(catalog->productsByName[i], entry->byName[i]);
        olRemove(catalog->productsByPrice[i], entry->byPrice[i]);
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
    if(config->orderAlphabetical) index = catalog->productsByName;
    else index = catalog->productsByPrice;
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
    if(!config->useFilter) relevant = catalog->productsByName[0];
    else relevant = catalog->productsByName[config->categoryFilter + 1];
    return (ProductEntry *) olValue(olSupremum(relevant, prefix));
}

ProductEntry * catPriceSupremum(Catalog * catalog, ListingConfig * config, Price price) {
    OrderedList * relevant;
    if(!config->useFilter) relevant = catalog->productsByPrice[0];
    else relevant = catalog->productsByPrice[config->categoryFilter + 1];
    return (ProductEntry *) olValue(olSupremum(relevant, (void *) price));
}

ProductEntry * catLookup(Catalog * catalog, ListingConfig * config, ProductId id) {
    return (ProductEntry *) dictValue(dictLookup(catalog->productsById, id));
}
