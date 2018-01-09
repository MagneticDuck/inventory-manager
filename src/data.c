#include "data.h"


void ppRecord(ProductRecord * record) {
    printf("%s | " FORMAT_PRICE " | %s", record->name, record->price, record->category->name);
}


typedef struct {
    size_t lineNumber;
    bool definingCategories;
    Category * categories[MAX_CATEGORIES];
    size_t categoryCount;

    int currentRecordField;
    ProductRecord * parsedRecord;
} ParserState;

// Sanitize maybe?
bool readString(size_t lineNumber, char * dest, char * line) {
    unimplemented();
}

bool readInt(size_t lineNumber, int * dest, char * line) {
    unimplemented();
}

bool stepParser(ParserState * state, char * line, void * catcher,
                bool (*onCategory)(void *, Category *),
                bool (*onRecord)(void *, ProductRecord *)) {
    ++state->lineNumber;
    // Category definition?
    if(strncmp(line, "CAT ", 4) == 0) {
        if(!state->definingCategories) {
            printf("Category definitions are not consecutive and at start of file!\n"
                   "Found out-of-place CAT on line %i.", state->lineNumber);
            return false;
        }
        if(state->categoryCount == MAX_CATEGORIES) {
            printf("Too many product categories! Maximum is %i. (Sorry.)", MAX_CATEGORIES);
            return false;
        }
        Category * newCategory = (state->categories[state->categoryCount] = malloc(sizeof(Category)));
        newCategory->code = state->categoryCount;
        strcpy(newCategory->name, line[4]);
        onCategory(catcher, newCategory);
        ++state->categoryCount;
        return true;
    }

    // Record field definition?
    {
        state->definingCategories = false;
        if(!state->parsedRecord) {
            state->currentRecordField = 0;
            state->parsedRecord = malloc(sizeof(ProductRecord));
        }

        switch(state->currentRecordField++) {
        case 0:
            return readString(state->lineNumber, state->parsedRecord->id, line);
        case 1:
            return readInt(state->lineNumber, &state->parsedRecord->instances, line);
        case 2:
            return readString(state->lineNumber, state->parsedRecord->name, line);
        case 3:
            return readInt(state->lineNumber, &state->parsedRecord->price, line);
        case 4: {
            CategoryCode code;
            if(!readInt(state->lineNumber, &code, line)) return false;
            if(code > state->categoryCount - 1) {
                printf("Category code out of bounds on line %i.", state->lineNumber);
                return false;
            }
            state->parsedRecord->category = &state->categories[code];
            onRecord(catcher, state->parsedRecord);
            return true;
        }
        }
    }
}

bool loadFile_(
    Filepath filepath, void * catcher, bool expectCategories,
    bool (*onCategory)(void *, Category *),
    bool (*onRecord)(void *, ProductRecord *)) {
    FILE * file = fopen(filepath, "r");
    if(file) {
        ParserState state;
        state.lineNumber = 0;
        state.categoryCount = 0;
        state.definingCategories = expectCategories;
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

bool readCatalogFile(
    Filepath filepath, void * catcher,
    bool (*onReadCategory)(void *, Category *),
    bool (*onReadRecord)(void *, ProductRecord *)) {
    return loadFile_(filepath, catcher, true, onReadCategory, onReadRecord);
}

void randomProductID_(ProductID id) {
    randomWordFixed(PRODUCT_ID_LENGTH - 1, id);
}

void randomRecord_(ProductRecord * record, CategoryCode categoryCount) {
    randomWord(record->name);
    randomProductID(record->id);
    record->category = randomIntRange(0, categoryCount);
    record->price = randomIntRange(0, 10000);
    record->instances = randomIntRange(1, 50);
}

bool readRandomCatalog(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (* onReadCategory)(void *, Category *),
    bool (* onReadRecord)(void *, ProductRecord *)) {
    Category ** categories = malloc(sizeof(char) * categoryCount);
    for(size_t i = 0; i <= categoryCount; ++i) {
        randomWord(categories[i]);
        onReadCategory(catcher, categories[i]);
    }
    for(size_t i = 0; i <= recordCount; ++i) {
        ProductRecord * record = malloc(sizeof(ProductRecord));
        randomRecord(record, categoryCount);
        onReadRecord(catcher, record);
    }
    return true;
}

bool readRecordFile(
    Filepath filepath, void * catcher,
    bool (*onReadRecord)(void *, ProductRecord *)) {
    return loadFile_(filepath, catcher, false, NULL, onReadRecord);
}

bool writeFile(
    Filepath filepath, void * iterator,
    bool (popCategory)(void *, Category **),
    bool (popRecord)(void *, ProductRecord **)) {
    FILE * file = fopen(filepath, "w");
    {
        Category * category;
        while(popCategory(iterator, &category)) {
            printf("writing category %s\n", category->name);
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



