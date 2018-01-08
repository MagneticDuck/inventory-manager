#include "data.h"

ReadStatus readFlatfile(
    Filepath filepath, void * catcher,
    bool (*onCategory)(Category *, void *),
    bool (*onRecord)(ProductRecord *, void *)) {
    FILE * file = fopen(filepath, "r");
    // TODO
}

ReadStatus readDemo(
    void * catcher,
    bool (* onDefCategory)(void *, Category *),
    bool (* onDefRecord)(void *, ProductRecord *)) {
    for(size_t i = 0; i <= 10; ++i) {
        Category * category = malloc(sizeof(Category));
        category->code = i;
        getRandomName(category->name);
        onDefCategory(catcher, category);
    }
    for(size_t i = 0; i <= 10000; ++i) {
        ProductRecord * record = malloc(sizeof(ProductRecord));
        // stuff
        onDefRecord(catcher, record);
    }
    return READ_OK;
}

ProductRecord newRandomRecord() {

}

void freeRecord(ProductRecord *) {

}

ReadStatus readDemo(void * catcher, bool (* onDefCategory)(Category *, void *), bool (* onDefRecord)(ProductRecord *, void *)) {

}

WriteStatus writeFlatfile(Filepath filepath, void * iterator, bool (popCategory)(void *, Category **), bool (popRecord)(void *, ProductRecord **)) {

}



