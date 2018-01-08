#include "data.h"

void randomProductID(ProductID id) {
    randomWordFixed(PRODUCT_ID_LENGTH - 1, id);
}

void ppCategory(Category * category) {
    printf("%s (%i)\n", category->name, category->code);
}

Category * newRandomCategory(unsigned char code) {
    Category * category = malloc(sizeof(Category));
    randomWord(category->name);
    category->code = code;
    return category;
}

void ppRecord(ProductRecord * record) {
    printf("%s | %i | %s", record->name, record->price, record->category->name);
}

ProductRecord * newRandomRecord(Category * category) {
    ProductRecord * record = malloc(sizeof(ProductRecord));
    randomWord(record->name);
    record->category = category;
    record->price = randomIntRange(0, 10000);
    record->instances = randomIntRange(1, 50);
    return record;
}

ReadStatus readFlatfile(
    Filepath filepath, void * catcher,
    bool (*onCategory)(void *, Category *),
    bool (*onRecord)(void *, ProductRecord *)) {
    FILE * file = fopen(filepath, "r");
    // TODO
}

ReadStatus readRandom(
    size_t categories, size_t records, void * catcher,
    bool (* onDefCategory)(void *, Category *),
    bool (* onDefRecord)(void *, ProductRecord *)) {
    for(size_t i = 0; i <= 10; ++i) {
        Category * category = newRandomCategory();
        onDefCategory(catcher, category);
    }
    for(size_t i = 0; i <= 10000; ++i) {
        ProductRecord * record = malloc(sizeof(ProductRecord));
        // stuff
        onDefRecord(catcher, record);
    }
    return READ_OK;
}

ReadStatus readDemo(void * catcher, bool (* onDefCategory)(Category *, void *), bool (* onDefRecord)(ProductRecord *, void *)) {

}

WriteStatus writeFlatfile(Filepath filepath, void * iterator, bool (popCategory)(void *, Category **), bool (popRecord)(void *, ProductRecord **)) {

}



