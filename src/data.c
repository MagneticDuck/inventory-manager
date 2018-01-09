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

typedef struct {
    bool definingCategories;
    Category * categories[MAX_CATEGORIES];
    size_t categoryCount;

    int currentRecordField;
    ProductRecord * parsedRecord;
} ParserState;

bool stepParser(ParserState * state, char * line, void * catcher,
                bool (*onCategory)(void *, Category *),
                bool (*onRecord)(void *, ProductRecord *)) {
    if(strncmp(line, "CAT ", 4) == 0) {
        if(!state->definingCategories) {
            printf("Category definitions are not consecutive and at start of file!");
            return false;
        }
        if(state->categoryCount == MAX_CATEGORIES) {
            printf("Too many product categories! Maximum is 100. (Sorry.)");
            return false;
        }
        Category * newCategory = (state->categories[state->categoryCount] = malloc(sizeof(Category)));
        newCategory->code = state->categoryCount;
        strcpy(newCategory->name, line[4]);
        onCategory(catcher, newCategory);
        ++state->categoryCount;
        return true;
    }
    state->definingCategories = false;
}

bool loadFlatfile(
    Filepath filepath, void * catcher,
    bool (*onCategory)(void *, Category *),
    bool (*onRecord)(void *, ProductRecord *)) {
    FILE * file = fopen(filepath, "r");
    if(file) {
        ParserState state;
        state.categoryCount = 0;
        state.definingCategories = true;
        state.currentRecordField = NULL;

        char line[256];
        while(fgets(line, 256, file)) {
            if(!stepParser(&state, line, catcher, onCategory, onRecord))
                return false;
        }
        fclose(file);
        return true;
    }

    printf("Could not open file!\n");
    return false;
}

bool loadRandom(
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
        randomRecord(record, &categories[randomIntRange(0, categoryCount - 1)]);
        onDefRecord(catcher, record);
    }
    return true;
}

bool writeFlatfile(
    Filepath filepath, void * iterator,
    bool (popCategory)(void *, Category **),
    bool (popRecord)(void *, ProductRecord **)) {
    FILE * file = fopen(filepath, "w");
    {
        Category * category;
        while(popCategory(iterator, &category)) {
            fprintf(file, "CAT %s\n", category->name);
        }
    }
    {
        ProductRecord * record;
        #define STRING "%s\n"
        #define INT "%i\n"
        while(popRecord(iterator, &record)) {
            fprintf(STRING INT STRING INT INT,
                record->id,
                record->instances,
                record->name,
                record->price,
                record->category->code);
        }
        #undef STRING
        #undef INT
    }
}



