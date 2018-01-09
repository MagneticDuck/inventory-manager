#include "data.h"

typedef struct {
    size_t lineNumber;
    bool definingCategories;

    int currentRecordField;
    ProductRecord * parsedRecord;
} ParserState;

// Sanitize maybe?
bool readString(size_t lineNumber, char * dest, char * line) {
    unimplemented();
}

// Return true if things went well, otherwise display a message with lineNumber and return false.
bool readInt(size_t lineNumber, int * dest, char * line) {
    unimplemented();
}

bool stepParser(ParserState * state, char * line, void * catcher,
                bool (*onCategory)(void *, char *),
                bool (*onRecord)(void *, ProductRecord *)) {
    ++state->lineNumber;
    // Category definition?
    if(strncmp(line, "CAT ", 4) == 0) {
        if(!state->definingCategories) {
            printf("Category definitions are not consecutive and at start of file!\n"
                   "Found out-of-place CAT on line %zu.", state->lineNumber);
            return false;
        }
        char * name = malloc(sizeof(char) * MAX_STRING_LENGTH);
        strcpy(name, &line[4]);
        onCategory(catcher, name);
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
        default: {
            CategoryCode code;
            if(!readInt(state->lineNumber, &code, line)) return false;
            state->parsedRecord->category = code;
            onRecord(catcher, state->parsedRecord);
            return true;
        }
        }
    }
}

bool loadFile_(
    Filepath filepath, void * catcher, bool expectCategories,
    bool (*onCategory)(void *, char *),
    bool (*onRecord)(void *, ProductRecord *)) {
    FILE * file = fopen(filepath, "r");
    if(file) {
        ParserState state;
        state.lineNumber = 0;
        state.definingCategories = expectCategories;
        state.currentRecordField = 0;
        state.parsedRecord = NULL;

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
    bool (*onReadCategory)(void *, char *),
    bool (*onReadRecord)(void *, ProductRecord *)) {
    return loadFile_(filepath, catcher, true, onReadCategory, onReadRecord);
}

void randomRecord_(ProductRecord * record, CategoryCode categoryCount) {
    randomWord(record->name);
    randomWordFixed(PRODUCT_ID_LENGTH, record->id);
    record->category = randomIntRange(0, categoryCount);
    record->price = randomIntRange(0, 10000);
    record->instances = randomIntRange(1, 50);
}

bool readRandomCatalog(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (* onReadCategory)(void *, char *),
    bool (* onReadRecord)(void *, ProductRecord *)) {
    for(size_t i = 0; i <= categoryCount; ++i) {
        char * name = malloc(sizeof(char) * MAX_STRING_LENGTH);
        randomWord(name);
        onReadCategory(catcher, name);
    }
    for(size_t i = 0; i <= recordCount; ++i) {
        ProductRecord * record = malloc(sizeof(ProductRecord));
        randomRecord_(record, categoryCount);
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
    bool (popCategory)(void *, char **),
    bool (popRecord)(void *, ProductRecord **)) {
    FILE * file = fopen(filepath, "w");
    if (!file) {
        printf("Could not open file for writing!");
        return false;
    }
    {
        char * name;
        while(popCategory(iterator, &name)) {
            fprintf(file, "CAT %s\n", name);
        }
    }
    {
        ProductRecord * record;
#define STRING "%s\n"
#define INT "%i\n"
        while(popRecord(iterator, &record)) {
            fprintf(file, STRING INT STRING INT INT,
                    record->id,
                    record->instances,
                    record->name,
                    record->price,
                    record->category);
        }
#undef STRING
#undef INT
    }
    return true;
}
