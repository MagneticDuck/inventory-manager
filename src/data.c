#include "data.h"

void randomProductID(ProductID id) {
    randomWordFixed(PRODUCT_ID_LENGTH - 1, id);
}

void ppCategory(Category * category) {
    printf("%s (%i)\n", category->name, category->code);
}

void randomCategory(Category * category, unsigned char code) {
    randomWord(category->name);
    category->code = code;
}

void ppRecord(ProductRecord * record) {
    printf("%s | %i | %s", record->name, record->price, record->category->name);
}

void randomRecord(ProductRecord * record, Category * category) {
    randomWord(record->name);
    randomProductID(record->id);
    record->category = category;
    record->price = randomIntRange(0, 10000);
    record->instances = randomIntRange(1, 50);
}

ReadStatus readFlatfile(
    Filepath filepath, void * catcher,
    bool (*onCategory)(void *, Category *),
    bool (*onRecord)(void *, ProductRecord *)) {
    FILE * file = fopen(filepath, "r");
    // TODO
}

ReadStatus readRandom(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (* onDefCategory)(void *, Category *),
    bool (* onDefRecord)(void *, ProductRecord *)) {
    Category * categories = malloc(sizeof(Category) * categoryCount);
    for(size_t i = 0; i <= categoryCount; ++i) {
        randomCategory(&categories[i], i);
        onDefCategory(catcher, &categories[i]);
    }
    for(size_t i = 0; i <= recordCount; ++i) {
        ProductRecord * record = malloc(sizeof(ProductRecord));
        randomRecord(record, &categories[randomIntRange(0, categories - 1)]);
        onDefRecord(catcher, record);
    }
    return READ_OK;
}

ReadStatus readDemo(void * catcher, bool (* onDefCategory)(Category *, void *), bool (* onDefRecord)(ProductRecord *, void *)) {

}

WriteStatus writeFlatfile(Filepath filepath, void * iterator, bool (popCategory)(void *, Category **), bool (popRecord)(void *, ProductRecord **)) {

}



