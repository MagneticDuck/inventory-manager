#include<stdio.h>
#include<stdlib.h>
#include "catalog.h"
#include "util.h"
#include "interact.h"

// Viewing and editing product records.
void displayRecordDetail(Catalog *, ProductRecord * record, size_t line, char * buffer);
void tryEditRecordDetail(Catalog *, ProductRecord * record, size_t option, char * command, bool lockFields);
void serveProductEntry(Catalog *, CursesState *, ProductEntry *);

// Product listing interface.
void serveListing(Catalog *, CursesState *, ListingConfig *, ProductEntry *);

// UI scenes.
void serveMain(Catalog *, CursesState *);
void serveSelectListing(Catalog *, CursesState *);
void serveValues(Catalog *, CursesState *);
void serveAddProduct(Catalog *, CursesState *);

int main(void) {
    Catalog * catalog;
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
    if (line == 5) sprintf(buffer, "Remover Produto (NAO REVERSIVEL)");
    if (line < 5) displayRecordDetail(helper->catalog, catProductRecord(helper->entry), line, buffer);
}

void serveProductEntry(Catalog * catalog, CursesState * curses, ProductEntry * entry) {
    ProductEntryHelper helper;
    helper.catalog = catalog; 
    helper.entry = entry;
    ScrollState state = initialScrollState(); 
    loop {
        InteractResult result = interactVirtual((void *) &helper, curses,
            &displayProductEntryInterface, 6, state);
        state = result.state;
        if (result.hasCommand && result.option < 5) {
          tryEditRecordDetail(catalog, catProductRecord(entry), result.option, result.command, true);
          catRegisterRecordEdits(catalog, entry);
          continue;
        }
        /* TODO: deleting products
        if (!result.hasCommand && result.option == 5) {
        }
        */
        if (result.isQuit) return;
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
    size_t index = catIndex(config, entry);
    ScrollState state = startScrollAt(curses, index, catRecordCount(catalog, config));
    loop {
        ListingHelper helper;
        helper.catalog = catalog;
        helper.config = config;
        helper.currentEntry = entry;
        helper.currentIndex = index;
        InteractResult result = interactVirtual((void *) &helper, curses, &displayProducts, catRecordCount(catalog, config), state);
        state = result.state;
        if (result.isQuit) return;
        if (result.hasCommand) continue; // TODO: seeking and so on
        serveProductEntry(catalog, curses, catSeekBy(config, helper.currentEntry, (int) result.option - (int) helper.currentIndex));
    }
}

//// serveMain

void serveMain(Catalog * catalog, CursesState * curses) {
    ScrollState state = initialScrollState();
    loop {
        char menu[6][MAX_STRING_LENGTH] = 
          {"Ver / Editar Produtos",           // 0
           "Consultar Valores Totais",        // 1
           "Introduzir um Novo Produto",      // 2
           "Guardar", // 3
           "Guardar e Sair",                  // 4
           "Sair sem Guardar"                 // 5
          };
        InteractResult result = interact(curses, menu, 6, state);
        state = result.state;
        if (result.hasCommand || result.isQuit) continue;
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
        case 3:
            writeCatalog(catalog, "data/user.txt");
            break;
        case 4:
            writeCatalog(catalog, "data/user.txt");
            return;
        case 5:
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
    loop {
        loop {
            InteractResult result = 
                interactVirtual((void *) catalog, curses, &displayCategoryFilters, 
                  catCategoryCount(catalog) + 1, initialScrollState());
            if(result.isQuit) return;
            if(result.option == 0) config.useFilter = false;
            else {
                config.useFilter = true;
                config.categoryFilter = result.option - 1;
            }
            break;
        }

        char sortMenu[2][MAX_STRING_LENGTH] = {"Ordenar alfabeticamente", "Ordenar por precos"};
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

void serveAddProduct(Catalog * catalog, CursesState * curses) {
  ScrollState state = initialScrollState();
  AddProductHelper helper;
  helper.catalog = catalog;
  randomWordFixed(20, helper.record.id);
  strcpy(helper.record.name, "NAME");
  helper.record.price = 0;
  helper.record.category = 0;
  helper.record.instances = 0;

  loop {
    InteractResult result = interactVirtual((void *) &helper, curses, &displayAddProductInterface, 7, state);
    state = result.state;
    if (result.command && result.option < 5) tryEditRecordDetail(catalog, &helper.record, result.option, result.command, false);
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

