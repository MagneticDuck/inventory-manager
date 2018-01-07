#include "data.h"
#include "catalog.h"

typedef enum {
    CMD_FORWARD,
    CMD_BACK,
    CMD_SEEK,
    CMD_SEARCH,
    CMD_FILTER,
    CMD_EDIT,
    CMD_DELETE,
    CMD_HELP
} ListingCommandType;

typedef struct {
    ProductCatalog *catalog;
    ProductEntry *head;
    CategoryFilter filter;

    // more stuff
} ListingState;

void displayListing(ListingState *state) {
    // TODO: API for finding height of screen
    ProductEntry *entry = state->head;
    for (int i = 10; i > 0 && entry; --i) {
        printEntry(entry);
    }
    printf(">");
}

void comForward(ListingState *state) {
    *state->entry = catNextProductAlphabetical(state->entry, state->filter);
}

interpretInput(ListingState *state, char input[]) {
    switch (input[0]) {
        // blah blah
        'j':
    }
    return 0;
}

void displayListing(void) {
    ProductCatalog *catalog = demoCatalog();

    ListingState state;
    state.catalog = catalog;
    state.head = catFirst(catalog);

    state.filter.isEffective = false;
    state.filter.category = 0;

    loop {
        displayListing(&state);
        char input[MAX_STRING_LENGTH];
        scanf("%s", input);
        if (interpretInput(&state, input)) break;
    }

    deleteCatalog(catalog);
}
