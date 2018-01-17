#include<stdio.h>
#include<stdlib.h>
#include "catalog.h"
#include "util.h"
#include "interact.h"

void serveListing(Catalog *, CursesState *, ListingConfig *, ProductEntry *);

void serveMain(Catalog *, CursesState *);
void serveSelectListing(Catalog *, CursesState *);
void serveValues(Catalog *, CursesState *);
void serveAddProduct(Catalog *, CursesState *);
void serveReadProducts(Catalog *, CursesState *);

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

//// serveMain

void serveMain(Catalog * catalog, CursesState * curses) {
    ScrollState state = initialScrollState();
    loop {
        char menu[6][MAX_STRING_LENGTH] = 
          {"Ver / Editar Produtos",           // 0
           "Consultar Valores Totais",        // 1
           "Introduzir um Novo Produto",      // 2
           "Ler Novos Produtos dum Ficheiro", // 3
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
            serveReadProducts(catalog, curses);
            break;
        case 4:
            writeCatalog(catalog, "data/user.txt");
            return;
        case 5:
            return;
        }
    }
}

////

typedef struct {
    Catalog * catalog;
    ListingConfig * config;
    ProductEntry * currentEntry;
    size_t currentIndex;
} ListingHelper;

void displayProducts(void * ptr, size_t line, char * buffer) {
    ListingHelper * helper = (ListingHelper *) ptr;
    helper->currentEntry = catSeekBy(helper->config, helper->currentEntry, (int) line - (int) helper->currentIndex);
    helper->currentIndex = line;
    ppRecord(buffer, helper->catalog, helper->currentEntry); 
}

void serveListing(Catalog * catalog, CursesState * curses, 
      ListingConfig * config, ProductEntry * entry) {
    ScrollState state = initialScrollState();
    loop {
        ListingHelper helper;
        helper.catalog = catalog;
        helper.config = config;
        helper.currentEntry = entry;
        helper.currentIndex = 0;
        InteractResult result = interactVirtual((void *) &helper, curses, &displayProducts, catRecordCount(catalog, config), state);
        if (result.isQuit) return;
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

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
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
        if (result.isQuit) return;
    }    
}

//// serveAddProduct

typedef struct {
  Catalog * catalog;
  ProductRecord record;
} AddProductHelper;

void displayRecordDetail(Catalog * catalog, ProductRecord * record, size_t line, char * buffer) {
#define FIELD_FORMAT "%-20s: "
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

void displayAddProductInterface(void * ptr, size_t line, char * buffer) {
    AddProductHelper * helper = (AddProductHelper *) ptr;
    if (5 < line) displayRecordDetail(helper->catalog, &helper->record, line, buffer);
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

//// serveReadProducts

void serveReadProducts(Catalog * catalog, CursesState * curses) {

}

/*
void displayData(Catalog * catalog) {
    CategoryCode mostValuable = catCategoryByRank(catalog, 0);
    printf("A categoria mais valiosa e %s com %i unidades de preco em total.\n", catCategoryName(catalog, mostValuable), catCategoryValue(catalog, mostValuable));
    printf("Os tres produtos mais valiosas sao:\n");

    ListingConfig priceConfig;
    priceConfig.useFilter = 0;
    priceConfig.orderAlphabetical = false;
    ProductEntry * valuable = catFirst(catalog, &priceConfig);
    for(size_t i = 0; i < 3 && valuable; ++i) {
        ppRecord(catalog, valuable);
        valuable = catNext(&priceConfig, valuable);
    }
}
*/
#pragma GCC diagnostic pop

