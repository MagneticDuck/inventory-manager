#include<stdio.h>
#include<stdlib.h>
#include "catalog.h"
#include "util.h"
#include "interact.h"

void serveMain(Catalog *, CursesState *);
void serveListing(Catalog *, CursesState *);
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
        if (result.isQuit) continue;
        switch(result.option) {
        case 0:
            serveListing(catalog, curses);
            break;
        case 1:
            serveValues(catalog, curses);
            break;
        case 2:
            serveAddProduct(catalog, curses);
            return;
        case 3:
            serveReadProducts(catalog, curses);
        case 4:
            writeCatalog(catalog, "data/user.txt");
            return;
        case 5:
            return;
        }
    }
}

//// serveListing

void displayCategoryFilters(void * ptr, size_t line, char * buffer) {
    Catalog * catalog = (Catalog *) ptr;
    if(line == 0) strcpy(buffer, "(todos os produtos)");
    else strcpy(buffer, catCategoryName(catalog, line - 1));
}

typedef struct {
    Catalog * catalog;
    ListingConfig * config;
    ProductEntry * currentEntry;
    size_t currentIndex;
} ListingHelper;

void displayProducts(void * ptr, size_t line, char * buffer) {
    ListingHelper * helper = (ListingHelper *) ptr;
    helper->currentEntry = catSeekBy(helper->config, helper->currentEntry, line - helper->currentIndex);
    sprintf(buffer, "product %s", catProductRecord(helper->currentEntry)->name);
}

void serveListing(Catalog * catalog, CursesState * state) {
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic push
    ListingConfig config;
start:
    loop {
        InteractResult result = interactVirtual((void *) catalog, state, &displayCategoryFilters, catCategoryCount(catalog), initialScrollState());
        if(result.isQuit) return;
        if(result.option == 0) config.useFilter = false;
        else {
            config.useFilter = true;
            config.categoryFilter = result.option - 1;
        }
    }
    loop {
        char sortMenu[3][MAX_STRING_LENGTH] = {"Ordenar alfabeticamente", "Ordenar por precos"};
        InteractResult result = interact(state, sortMenu, 3, initialScrollState());
        if(result.isQuit) goto start;
        return;
    }
}

//// serveValues

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
void serveValues(Catalog * catalog, CursesState * state) {
     
}

//// serveAddProduct

void serveAddProduct(Catalog * catalog, CursesState * state) {

}

//// serveReadProducts

void serveReadProducts(Catalog * catalog, CursesState * state) {

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

