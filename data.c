#include "data.h"
#include "util.h"

typedef struct {
    size_t lineNumber;
    bool definingCategories;

    int currentRecordField;
    ProductRecord * parsedRecord;
} ParserState;

// Sanitize maybe?
bool readString(size_t lineNumber, size_t maxLength, char * dest, char * line) {
    size_t length = strlen(line); // MICROSOFT NEWLINES!! on windows we need to cut off two characters from the end
    if (length - 1 > maxLength) {
        printf("String on line %lui is longer than maximum length of %lu.\n", lineNumber, maxLength);
        return false;
    }
    memcpy(dest, line, length - 1);
    line[length - 1] = '\0';
    return true;
}

// Return true if things went well, otherwise display a message with lineNumber and return false.
bool readInt(size_t lineNumber, int * dest, char * line) {
    if (1 != sscanf(line, "%i\n", dest)) {
        printf("Could not read integer at line %lu.\n", lineNumber);
        return false;
    }
    return true;
}

bool stepParser(ParserState * state, char * line, void * catcher,
                bool (*onCategory)(void *, char *),
                bool (*onRecord)(void *, ProductRecord *)) {
    ++state->lineNumber;
    // Category definition?
    if(strncmp(line, "CAT ", 4) == 0) {
        if(!state->definingCategories) {
            printf("Category definitions are not consecutive and at start of file.\n"
                   "Found out-of-place CAT on line %zu.\n", state->lineNumber);
            return false;
        }
        char * name = malloc(sizeof(char) * (MAX_NAME_LENGTH+ 1));
        if (strlen(line) - 5 > MAX_NAME_LENGTH) {
          printf("Category name on line %lu is longer than maximum length of %i.\n", state->lineNumber, MAX_NAME_LENGTH);
          return false;
        }
        strcpy(name, &line[4]);
        name[strlen(name) - 1] = '\0';
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
            return readString(state->lineNumber, PRODUCT_ID_LENGTH, state->parsedRecord->id, line);
        case 1:
            return readInt(state->lineNumber, &state->parsedRecord->instances, line);
        case 2:
            return readString(state->lineNumber, MAX_NAME_LENGTH, state->parsedRecord->name, line);
        case 3:
            return readInt(state->lineNumber, &state->parsedRecord->price, line);
        default: {
            if(!readInt(state->lineNumber, &state->parsedRecord->category, line)) return false;
            onRecord(catcher, state->parsedRecord);
            state->parsedRecord = NULL;
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
    if(!file) {
        printf("Could not open file!\n");
        return false;
    }
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

bool readCatalogFile(
    Filepath filepath, void * catcher,
    bool (*onReadCategory)(void *, char *),
    bool (*onReadRecord)(void *, ProductRecord *)) {
    return loadFile_(filepath, catcher, true, onReadCategory, onReadRecord);
}

void randomRecord_(ProductRecord * record, CategoryCode categoryCount) {
    randomName(record->name);
    randomWordFixed(PRODUCT_ID_LENGTH, record->id);
    record->category = randomIntRange(0, categoryCount);
    record->price = randomIntRange(0, 10000);
    record->instances = randomIntRange(1, 50);
}

bool readRandomCatalog(
    size_t categoryCount, size_t recordCount, void * catcher,
    bool (* onReadCategory)(void *, char *),
    bool (* onReadRecord)(void *, ProductRecord *)) {
    for(size_t i = 0; i < categoryCount; ++i) {
        char * name = malloc(sizeof(char) * MAX_STRING_LENGTH);
        randomName(name);
        onReadCategory(catcher, name);
    }
    for(size_t i = 0; i < recordCount; ++i) {
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
    if(!file) {
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
