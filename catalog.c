#include "array.h"
#include "catalog.h"
#include "dictionary.h"
#include "ordered_list.h"
#include "data.h"

typedef struct ProductEntry {
    ProductRecord * record, * syncedRecord;
    DictEntry * byId;
    OLNode * byName[2];
    OLNode * byPrice[2];
} ProductEntry;

void copyRecord(ProductRecord * dest, ProductRecord * src) {
    strcpy(dest->id, src->id);
    strcpy(dest->name, src->name);
    dest->price = src->price;
    dest->category = src->category;
    dest->instances = src->instances;
}

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

typedef struct Catalog {
    Price netValue;
  
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
    if(catalog->categoriesByValue) freeOrderedList(catalog->categoriesByValue);
    catalog->categoriesByValue = newOrderedList(&priceCompare);
    for (CategoryCode code = 0; code < catCategoryCount(catalog); ++code) {
        CategoryEntry * entry = catCategoryEntry_(catalog, code);
        entry->byValue = olAdd(catalog->categoriesByValue, (void *) &entry->netValue, (void *) entry);
    }
}

void catUnsortCategories_(Catalog * catalog) {
    if(catalog->categoriesByValue) freeOrderedList(catalog->categoriesByValue);
    catalog->categoriesByValue = NULL;
}

bool onReadCategory_(void * ptr, char * name) {
    Catalog * catalog = (Catalog *) ptr;

    // Make entry.
    CategoryEntry * entry = malloc(sizeof(CategoryEntry));
    entry->name = name;
    entry->netValue = 0;

    // Add entry to indexing structure.
    entry->code = arrayAppend(catalog->categoriesByCode, entry);

    // Create indexing structures for products in this category.
    catalog->productsByName[entry->code + 1] = newOrderedList(&lexiographicCompare);
    catalog->productsByPrice[entry->code + 1] = newOrderedList(&priceCompare);
    return true;
}

// Just copy the record pointer.
ProductEntry * catAddRecord_(Catalog * catalog, ProductRecord * record) {
    // Make entry.
    ProductEntry * entry = malloc(sizeof(ProductEntry));
    entry->syncedRecord = record;
    entry->record= malloc(sizeof(ProductRecord));
    copyRecord(entry->record, entry->syncedRecord);

    // Add entry to all the structures.
    entry->byId = dictAdd(catalog->productsById, (void *) record->id, (void *) entry);
    entry->byName[0] = olAdd(catalog->productsByName[0], (void *) record->name, (void *) entry);
    entry->byPrice[0] = olAdd(catalog->productsByPrice[0], (void *) &record->price, (void *) entry);
    entry->byName[1] = olAdd(catalog->productsByName[record->category + 1], (void *) record->name, (void *) entry);
    entry->byPrice[1] = olAdd(catalog->productsByPrice[record->category + 1], (void *) &record->price, (void *) entry);

    // Update value counters and unregister category sort.
    CategoryEntry * catEntry = catCategoryEntry_(catalog, record->category);
    catalog->netValue += record->price * record->instances;
    catEntry->netValue += record->price * record->instances;
    catUnsortCategories_(catalog);
    return entry;
}

bool onReadRecord_(void * ptr, ProductRecord * record) {
    Catalog * catalog = (Catalog *) ptr;

    if(record->category > catCategoryCount(catalog)) {
        printf("Category code out of bounds!\n");
        return false;
    }

    catAddRecord_(catalog, record);
    return true;
}

void newCatalog_(Catalog ** catalog) {
    *catalog = malloc(sizeof(Catalog));
    Catalog * catalog_ = *catalog;
  
    catalog_->netValue = 0;

    catalog_->categoriesByCode = newArray(MAX_CATEGORIES);
    catalog_->categoriesByValue = NULL;

    catalog_->productsById = newDictionary();
    catalog_->productsByName[0] = newOrderedList(&lexiographicCompare);
    catalog_->productsByPrice[0] = newOrderedList(&priceCompare);
}

bool newCatalogFromFile(Catalog ** catalog, Filepath filepath) {
    newCatalog_(catalog);
    return readCatalogFile(filepath, (void *) *catalog, &onReadCategory_, &onReadRecord_);
}

void newCatalogRandom(Catalog ** catalog, size_t categoryCount, size_t recordCount) {
    newCatalog_(catalog);
    readRandomCatalog(categoryCount, recordCount, (void *) *catalog, &onReadCategory_, &onReadRecord_);
}

void freeCatalog(Catalog * catalog) {
    freeDictionary(catalog->productsById);
    for(CategoryCode i = 0; i <= catCategoryCount(catalog); ++i) {
        freeOrderedList(catalog->productsByName[i]);
        freeOrderedList(catalog->productsByPrice[i]);
    }
    if(catalog->categoriesByValue) freeOrderedList(catalog->categoriesByValue);
    for(CategoryCode i = 0; i < catCategoryCount(catalog); ++i)
        freeCategoryEntry_(arrayAccess(catalog->categoriesByCode, i));
    freeArray(catalog->categoriesByCode);
    free(catalog);
}

typedef struct {
    Catalog * catalog;
    CategoryCode categoryIndex;
    ProductEntry * head;
} CatalogIterator;

bool popCategory_(void * ptr, char ** name) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(iterator->categoryIndex >= catCategoryCount(iterator->catalog)) return false;
    *name = catCategoryEntry_(iterator->catalog, iterator->categoryIndex++)->name;
    return true;
}

bool popRecord_(void * ptr, ProductRecord ** record) {
    CatalogIterator * iterator = (CatalogIterator *) ptr;
    if(!iterator->head) return false;
    *record = catProductRecord(iterator->head);
    iterator->head = catNext(NULL_CONFIG(), iterator->head);
    return true;
}

void writeCatalog(Catalog * catalog, char * filepath) {
    CatalogIterator iterator;
    iterator.catalog = catalog;
    iterator.head = catFirst(catalog, NULL_CONFIG());
    iterator.categoryIndex = 0;
    writeFile(filepath, (void *) &iterator, &popCategory_, &popRecord_);
}

void ppRecord(char * string, Catalog * catalog, ProductEntry * entry) {
    if (entry == NULL) sprintf(string, "<NULL>");
    else {
        ProductRecord * record = catProductRecord(entry);
        sprintf(string, "%s | " FORMAT_NAME " | %-5i | (%-5i em stock) | %s", 
          record->id, record->name,
          record->price, record->instances,
          catCategoryName(catalog, record->category));
    }
}

CategoryCode catCategoryCount(Catalog * catalog) {
    return arraySize(catalog->categoriesByCode);
}

char * catCategoryName(Catalog * catalog, CategoryCode code) {
    return catCategoryEntry_(catalog, code)->name;
}

Price catTotalValue(Catalog * catalog) {
  return catalog->netValue;
}

Price catCategoryValue(Catalog * catalog, CategoryCode code) {
    return catCategoryEntry_(catalog, code)->netValue;
}

CategoryCode catCategoryByRank(Catalog * catalog, size_t place) {
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

ProductEntry * catAddRecord(Catalog * catalog, ProductRecord * userRecord) {
    ProductRecord * record = malloc(sizeof(ProductRecord));
    copyRecord(record, userRecord);
    return catAddRecord_(catalog, record);
}

void catRegisterRecordEdits(Catalog * catalog, ProductEntry * entry) {
    // Update value stats, unregister category value sort.
    Price priceDelta = (entry->record->price * entry->record->instances) 
      - (entry->syncedRecord->price * entry->syncedRecord->instances);
    catalog->netValue += priceDelta;
    catCategoryEntry_(catalog, entry->record->category)->netValue += priceDelta;
    catUnsortCategories_(catalog);

    // Reindex ordered lists.
    copyRecord(entry->syncedRecord, entry->record);
    olReindex(catalog->productsByPrice[0], 
        entry->byPrice[0], &entry->syncedRecord->price);
    olReindex(catalog->productsByPrice[entry->record->category + 1], 
        entry->byPrice[1], &entry->syncedRecord->price);
    olReindex(catalog->productsByName[0],
         entry->byName[0], &entry->syncedRecord->name);
    olReindex(catalog->productsByName[entry->record->category + 1],
         entry->byName[1], &entry->syncedRecord->name);
}

void catUndoRecordEdits(ProductEntry * entry) {
  copyRecord(entry->record, entry->syncedRecord);
}

void catRemove(Catalog * catalog, ProductEntry * entry) {
    // Update value stats, unregister category value sort.
    Price priceDelta = entry->record->price * entry->record->instances;
    catalog->netValue -= priceDelta;
    catCategoryEntry_(catalog, entry->record->category)->netValue -= priceDelta;
    catUnsortCategories_(catalog);

    // Remove from indexing structures.
    dictRemove(catalog->productsById, entry->byId);
    olRemove(catalog->productsByName[0], entry->byName[0]);
    olRemove(catalog->productsByName[entry->record->category + 1], 
        entry->byName[1]);
    olRemove(catalog->productsByPrice[0], entry->byPrice[0]);
    olRemove(catalog->productsByPrice[entry->record->category + 1], 
        entry->byPrice[1]);

    // Free memory.
    free(entry->record);
    free(entry->syncedRecord);
}

OLNode * getRelevantNode_(ProductEntry * entry, ListingConfig * config) {
    OLNode ** index;
    if(config->orderAlphabetical) index = entry->byName;
    else index = entry->byPrice;
    if(config->useFilter) return index[1];
    else return index[0];
}

OrderedList * getRelevantList_(Catalog * catalog, ListingConfig * config) {
    OrderedList ** index;
    if(config->orderAlphabetical) index = catalog->productsByName;
    else index = catalog->productsByPrice;
    if(!config->useFilter) return index[0];
    else return index[config->categoryFilter + 1];
}

size_t catRecordCount(Catalog * catalog, ListingConfig * config) {
    return olSize(getRelevantList_(catalog, config));
}

size_t catIndex(ListingConfig * config, ProductEntry * entry) {
    return olIndex(getRelevantNode_(entry, config));
}

ProductEntry * catFirst(Catalog * catalog, ListingConfig * config) {
    return (ProductEntry *) olValue(olFirst(getRelevantList_(catalog, config)));
}

ProductEntry * catLast(Catalog * catalog, ListingConfig * config) {
    return (ProductEntry *) olValue(olLast(getRelevantList_(catalog, config)));
}

ProductEntry * catNext(ListingConfig * config, ProductEntry * entry) {
    return (ProductEntry *) olValue(olNext(getRelevantNode_(entry, config)));
}

ProductEntry * catPrev(ListingConfig * config, ProductEntry * entry) {
    return (ProductEntry *) olValue(olPrev(getRelevantNode_(entry, config)));
}

ProductEntry * catSeekBy(ListingConfig * config, ProductEntry * entry, int seeking) {
    return (ProductEntry *) olValue(olSeekBy(getRelevantNode_(entry, config), seeking));
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
    return (ProductEntry *) olValue(olSupremum(relevant, (void *) &price));
}
