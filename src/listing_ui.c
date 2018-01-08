#include "data.h"
#include "catalog.h"

typedef struct {
    Catalog * catalog;
    ProductEntry * head;
    ListingConfig * config;
    int consoleHeight;
} ListingState;

void printEntry(ListingState * state, ProductEntry * entry) {
    printf("%s", catGetProductID(entry));
    printf("%s", catGetRecord(entry)->name);
}

void displayListing(ListingState * state) {
    // TODO: API for finding height of screen
    ProductEntry * entry = state->head;
    for(int i = 10; i > 0 && entry; --i) {
        printEntry(state, entry);
        entry = catGetNext(state->catalog, state->config, entry);
    }
    printf(">");
}

typedef enum {
    INTERPRET_OK = 0,
    INTERPRET_MESSAGE,
    INTERPRET_QUIT
} InterpretResult;

InterpretResult helpfile(ListingState * state, char input[]) {
    printf("This is the product listing interface.");
    // TODO: read out state->config here
    // TODO: give overview of commands
    return INTERPRET_MESSAGE;
}

InterpretResult performSeek(ListingState * state, int dir, char input[]) {
    int seeking = dir;
    if(strlen(input) > 1) {
        // TODO: let commands in the form 'f <n>' seek forward by n entries.
        printf("Seeking by values is not implemented yet.");
        // seeking *= n; ...
        return INTERPRET_MESSAGE;
    }
    if(!catSeekBy(state->catalog, state->config, state->head, state->consoleHeight + seeking)) {
        if(seeking == 1) printf("Already at end of listing!\n");
        if(seeking == -1) printf("Already at start of listing!\n");
        else printf("Cannot seek that far!\n");
        return INTERPRET_MESSAGE;
    }
    state->head = catSeekBy(state->catalog, state->config, state->head, seeking);
    return INTERPRET_OK;
}


// Returns 1 if a message was displayed and we should wait for a newline from stdin to refresh.
InterpretResult interpretInput(ListingState * state, char input[]) {
    switch(input[0]) {
    case 'h': {
        return helpfile(state, input);
    }
    case 'f': {
        return performSeek(state, 1, input);
    }
    case 'b': {
        return performSeek(state, -1, input);
    }
    case 'q': {
        return INTERPRET_QUIT;
    }
    default: {
        printf("Your command was not recognized. Try 'h' for help.");
        return 1;
    }
    }
    return INTERPRET_OK;
}

void interactListing(Catalog * catalog, ListingConfig * config) {
    ListingState state;
    state.catalog = catalog;
    state.config = config;
    state.head = catFirst(catalog);
    state.consoleHeight = 10; // TODO: find real console height

    loop {
        displayListing(&state);
        char input[MAX_STRING_LENGTH];
        scanf("%s", input);
        InterpretResult result;
        if(result = interpretInput(&state, input)) {
            if(result == INTERPRET_MESSAGE) scanf("%s", input);
            if(result == INTERPRET_QUIT) break;
            // unreachable
        }
    }
}
