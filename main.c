#include<stdio.h>
#include<stdlib.h>
#include "catalog.h"
#include "util.h"
#include "interact.h"

// Viewing and editing product records.
void displayRecordDetail(Catalog *, ProductRecord * record, size_t line, char * buffer);
void tryEditRecordDetail(Catalog *, ProductRecord * record, size_t option, char * command, bool lockFields);
bool serveProductEntry(Catalog *, CursesState *, ProductEntry *);

// Product listing interface.
void serveListing(Catalog *, CursesState *, ListingConfig *, ProductEntry *);

// UI scenes.
void serveMain(Catalog *, CursesState *);
void serveSelectListing(Catalog *, CursesState *);
void serveValues(Catalog *, CursesState *);
void serveAddProduct(Catalog *, CursesState *);

int main(void) {
    initRandomSeed(); // for product IDs
    Catalog * catalog;
    newCatalogRandom(&catalog, 10, 10000);
    writeCatalog(catalog, "data/random.txt");
    freeCatalog(catalog);

    if(!newCatalogFromFile(&catalog, "data/user.txt")) {
        printf("\n(Press enter to exit.)");
        awaitNewline(1);
        return 1;
    }
    CursesState * curses = openCurses();
    serveMain(catalog, curses);
    freeCatalog(catalog);
    closeCurses(curses);
    return 0;
}

//// Utilities.

void displayRecordDetail(Catalog * catalog, ProductRecord * record, size_t line, char * buffer) {
#define FIELD_FORMAT "> %-20s: "
    switch (line) {
    case 0:
        sprintf(buffer, FIELD_FORMAT "%s", "id", record->id);
        break;
    case 1:
        sprintf(buffer, FIELD_FORMAT "%s", "nome", record->name);
        break;
    case 2:
        sprintf(buffer, FIELD_FORMAT "%i", "preco", record->price);
        break;
    case 3:
        sprintf(buffer, FIELD_FORMAT "%i (%s)", "categoria", record->category, catCategoryName(catalog, record->category));
        break;
    case 4:
        sprintf(buffer, FIELD_FORMAT "%i", "no de existencias", record->instances);
        break;
    }
#undef FIELD_FORMAT
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
void tryEditRecordDetail(Catalog * catalog, ProductRecord * record, size_t option, char * command, bool lockFields) {
    switch (option) {
    case 0: // Can't edit ID, sorry :(
        break;
    case 1:
        strcpy(record->name, command);
        break;
    case 2: {
        Price price;
        if (sscanf(command, "%i", &price) == 1) record->price = price;
        break;
    }
    case 3: {
        if (lockFields) break;
        CategoryCode category;
        if (sscanf(command, "%i", &category) && category >= 0 && category < catCategoryCount(catalog))
            record->category = category;
        break;
    }
    case 4: {
        size_t instances;
        if (sscanf(command, "%lu", &instances) == 1) record->instances = instances;
        break;
    }
    }
}
#pragma GCC diagnostic pop

typedef struct {
    Catalog * catalog;
    ProductEntry * entry;
} ProductEntryHelper;

void displayProductEntryInterface(void * ptr, size_t line, char * buffer) {
    ProductEntryHelper * helper = (ProductEntryHelper *) ptr;
    if (line < 5) displayRecordDetail(helper->catalog, catProductRecord(helper->entry), line, buffer);
    if (line == 5) sprintf(buffer, "Efetua Alteracoes");
    if (line == 6) sprintf(buffer, "Cancela Alteracoes (q)");
    if (line == 7) sprintf(buffer, "Remover Produto (NAO REVERSIVEL)");
}

bool serveProductEntry(Catalog * catalog, CursesState * curses, ProductEntry * entry) {
    ProductEntryHelper helper;
    helper.catalog = catalog;
    helper.entry = entry;
    ScrollState state = initialScrollState();
    loop {
        InteractResult result = interactVirtual((void *) &helper, curses,
        &displayProductEntryInterface, 8, state);
        state = result.state;
        if (result.hasCommand && result.option < 5) {
            tryEditRecordDetail(catalog, catProductRecord(entry), result.option, result.command, true);
            continue;
        }
        if (result.option == 5)  {
            catRegisterRecordEdits(catalog, entry);
            return false;
        }
        if (result.isQuit || result.option == 6)  {
            catUndoRecordEdits(entry);
            return false;
        }
        if (!result.hasCommand && result.option == 7) {
            return true;
        }
    }
}

typedef struct {
    Catalog * catalog;
    ListingConfig * config;
    ProductEntry * currentEntry;
    size_t currentIndex;
} ListingHelper;

void displayProducts(void * ptr, size_t line, char * buffer) {
    ListingHelper * helper = (ListingHelper *) ptr;
    helper->currentEntry = catSeekBy(helper->config, helper->currentEntry,
                                     (int) line - (int) helper->currentIndex);
    helper->currentIndex = line;
    ppRecord(buffer, helper->catalog, helper->currentEntry);
}

void serveListing(Catalog * catalog, CursesState * curses,
                  ListingConfig * config, ProductEntry * entry) {
start:
    if (!entry) {
        loop {
            InteractResult result = interact(curses, NULL, 0, initialScrollState());
            if (result.isQuit) return;
        }
    }
    size_t index = catIndex(config, entry);
    ScrollState state = startScrollAt(curses, index, catRecordCount(catalog, config));
    loop {
        ListingHelper helper;
        helper.catalog = catalog;
        helper.config = config;
        helper.currentEntry = entry;
        helper.currentIndex = index;
        InteractResult result = interactVirtual(
            (void *) &helper, curses, &displayProducts,
            catRecordCount(catalog, config), state);
        state = result.state;
        if (result.isQuit) return;
        if (result.hasCommand) {
            // Try to search.
            ProductEntry * supremum = NULL;
            if (config->orderAlphabetical
            && (supremum = catAlphabeticalSupremum(catalog, config,
            result.command))) {
                entry = supremum;
                goto start;
            } else {
                Price price = 0;
                if (sscanf(result.command, "%i", &price) == 1
                        && (supremum = catPriceSupremum(catalog, config, price))) {
                    entry = supremum;
                    goto start;
                }
            }
        } else {
            // Open product details.
            entry = catSeekBy(config, helper.currentEntry,
            (int) result.option - (int) helper.currentIndex);
            if (serveProductEntry(catalog, curses, entry)) {
                // User requested to delete product.
                ProductEntry * dangling = entry;
                if (catNext(config, entry)) entry = catNext(config, entry);
                else if (catPrev(config, entry))
                    entry = catPrev(config, entry);
                else entry = NULL;
                catRemove(catalog, dangling);
            }
            goto start;
        }
    }
}

//// serveMain

void serveMain(Catalog * catalog, CursesState * curses) {
    ScrollState state = initialScrollState();
    loop {
#define MENU_SIZE 7
        char menu[MENU_SIZE][MAX_STRING_LENGTH] =
        {
            "Listar e Editar Produtos",        // 0
            "Consultar Valores Totais",        // 1
            "Introduzir um Novo Produto",      // 2
            "Prequisar por ID",                // 3
            "Guardar",                         // 4
            "Guardar e Sair",                  // 5
            "Sair sem Guardar"                 // 6
        };
        InteractResult result = interact(curses, menu, MENU_SIZE, state);
#undef MENU_SIZE
        state = result.state;
        if (result.isQuit) continue;
        if (result.hasCommand && result.option == 3) {
            ProductEntry * lookup = catLookupProduct(catalog, result.command);
            if (lookup) serveListing(catalog, curses, NULL_CONFIG(), lookup);
            continue;
        } else if (result.hasCommand) continue;
        switch(result.option) {
        case 0:
            serveSelectListing(catalog, curses);
            break;
        case 1:
            serveValues(catalog, curses);
            break;
        case 2:
            serveAddProduct(catalog, curses);
            break;
        case 4:
            writeCatalog(catalog, "data/user.txt");
            break;
        case 5:
            writeCatalog(catalog, "data/user.txt");
            return;
        case 6:
            return;
        }
    }
}

//// serveSelectListing

void displayCategoryFilters(void * ptr, size_t line, char * buffer) {
    Catalog * catalog = (Catalog *) ptr;
    if(line == 0) strcpy(buffer, "(todos os produtos)");
    else strcpy(buffer, catCategoryName(catalog, line - 1));
}

void serveSelectListing(Catalog * catalog, CursesState * curses) {
    ListingConfig config;
    ScrollState state = initialScrollState();
    loop {
        loop {
            InteractResult result =
            interactVirtual((void *) catalog, curses, &displayCategoryFilters,
            catCategoryCount(catalog) + 1, state);
            state = result.state;
            if(result.isQuit) return;
            if(result.option == 0) config.useFilter = false;
            else {
                config.useFilter = true;
                config.categoryFilter = result.option - 1;
            }
            break;
        }

        char sortMenu[2][MAX_STRING_LENGTH] = {"Ordenar Alfabeticamente", "Ordenar por Precos"};
        InteractResult result = interact(curses, sortMenu, 2, initialScrollState());
        if(result.isQuit) continue;
        config.orderAlphabetical = result.option == 0;
        serveListing(catalog, curses, &config, catFirst(catalog, &config));
    }
}

//// serveValues

void displayValues(void * ptr, size_t line, char * buffer) {
    Catalog *catalog = (Catalog *) ptr;
    if (line == 0) sprintf(buffer, FORMAT_NAME ": %i", "(em total)", catTotalValue(catalog));
    else {
        CategoryCode code = catCategoryByRank(catalog, line - 1);
        sprintf(buffer, FORMAT_NAME ": %i", catCategoryName(catalog, code), catCategoryValue(catalog, code));
    }
}

void serveValues(Catalog * catalog, CursesState * curses) {
    ScrollState state = initialScrollState();
    loop {
        InteractResult result = interactVirtual((void *) catalog, curses, &displayValues, catCategoryCount(catalog) + 1, state);
        state = result.state;
        if (result.isQuit) return;
    }
}

//// serveAddProduct

typedef struct {
    Catalog * catalog;
    ProductRecord record;
} AddProductHelper;

void displayAddProductInterface(void * ptr, size_t line, char * buffer) {
    AddProductHelper * helper = (AddProductHelper *) ptr;
    if (line < 5) displayRecordDetail(helper->catalog, &helper->record, line, buffer);
    if (line == 5) sprintf(buffer, "Concluir e Adicionar");
    if (line == 6) sprintf(buffer, "Cancelar Operacao");
}

void displayCategories(void * ptr, size_t line, char * buffer) {
    Catalog *catalog = (Catalog *) ptr;
    strcpy(buffer, catCategoryName(catalog, line));
}

void serveAddProduct(Catalog * catalog, CursesState * curses) {
    ScrollState state = initialScrollState();
    AddProductHelper helper;
    helper.catalog = catalog;
    randomWordFixed(20, helper.record.id);
    strcpy(helper.record.name, "");
    helper.record.price = 0;
    helper.record.category = 0;
    helper.record.instances = 0;

    loop {
        InteractResult result = interactVirtual((void *) &helper, curses, &displayAddProductInterface, 7, state);
        state = result.state;
        if (result.command && result.option < 5) tryEditRecordDetail(catalog, &helper.record, result.option, result.command, false);
        if (!result.command && result.option == 3) {
            InteractResult result = interactVirtual((void *) catalog, curses, &displayCategories, catCategoryCount(catalog), initialScrollState());
            if (!result.isQuit) helper.record.category = result.option;
        }
        if (result.option == 6 || result.isQuit) return;
        if (result.option == 5) {
            ProductEntry * entry = catAddRecord(catalog, &helper.record);
            ListingConfig config;
            config.orderAlphabetical = true;
            config.useFilter = false;
            serveListing(catalog, curses, &config, entry);
            return;
        }
    }
}

