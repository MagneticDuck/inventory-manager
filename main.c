#include<stdio.h>
#include<stdlib.h>
#include<curses.h>
#include "catalog.h"
#include "util.h"
#include "menu.h"

void serveMain(Catalog *, CursesState *);
void serveListings(Catalog *, CursesState *);
void serveData(Catalog *, CursesState *);

/*
int main(void) {
    Catalog * catalog;
    if(!newCatalogFromFile(&catalog, "data/random.txt")) {
        printf("(Press enter to exit.)");
        awaitNewline(2); // Display error.
        return 1;
    }
    CursesState * state = openCurses();
    serveMain(catalog, state);
    freeCatalog(catalog);
    return 0;
}
*/

int main(void) {
    CursesState * curses = openCurses();
    char list[200][MAX_STRING_LENGTH];
    for (size_t i = 0; i < 200; ++i) sprintf(list[i], "item %i", i);

    ScrollState state = initialScrollState();

    loop {
        InteractResult result = interact(curses, list, 200, state);
        state = result.state;
        if (result.hasCommand)
            strcpy(list[result.option], result.command);
    }

    closeCurses(curses);
}

void serveMain(Catalog * catalog, CursesState * state) {
    loop {
        char menu[3][MAX_STRING_LENGTH] = {"Listagem", "Dados", "Sair"};
        InteractResult result = interact(state, menu, 3, initialScrollState());
        switch(result.option) {
        case 0:
            serveListings(catalog, state);
            break;
        case 1:
            serveData(catalog, state);
            break;
        case 2:
            return;
        }
    }
}

void displayCategories(void * ptr, size_t line, char * buffer) {
    Catalog * catalog = (Catalog *) ptr;
    if(line == 0) strcpy(buffer, "(Voltar)");
    if(line == 1) strcpy(buffer, "todos os produtos");
    else strcpy(buffer, catCategoryName(catalog, line - 2));
}

typedef struct {
    Catalog * catalog;
    ListingConfig * config;
    ProductEntry * currentEntry;
    size_t currentIndex;
} ListingHelper;

void displayProducts(void * ptr, size_t line, char * buffer) {
    ListingHelper * helper = (ListingHelper *) ptr;
    helper->currentEntry = catSeekBy(helper->catalog, helper->config, helper->currentEntry, line - helper->currentIndex);
    sprintf(buffer, "product %s", catProductRecord(helper->currentEntry)->name);
}

void serveListings(Catalog * catalog, CursesState * state) {
    ListingConfig config;
start:
    loop {
        InteractResult result = interactVirtual((void *) catalog, state, &displayCategories, catCategoryCount(catalog) + 2, initialScrollState());
        if(result.option == 0) return;
        if(result.option == 1) config.useFilter = false;
        else {
            config.useFilter = true;
            config.categoryFilter = result.option - 1;
        }
    }
    loop {
        char sortMenu[3][MAX_STRING_LENGTH] = {"(Voltar)", "Ordenar alfabeticamente", "Ordenar por precos"};
        InteractResult result = interact(state, sortMenu, 3, initialScrollState());
        if(result.option == 0) goto start;
        return;
    }
}

void serveData(Catalog * catalog, CursesState * state) {

}

/*

WINDOW * create_newwin(int height, int width, int starty, int startx) {
    WINDOW * local_win;

    local_win = newwin(height, width, starty, startx);
    return local_win;
}

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
        valuable = catNext(catalog, &priceConfig, valuable);
    }
}

void enterKey(Catalog * catalog, WINDOW * win, int nchoice, int * ch) {
    int j, key, i, exit, startx, starty;
    char listagem[4][30] = {"Por ordem alfabetica", "Por Preco", "Por categoria alfabeticamente", "Voltar"};
    char dados[4][30] = {"3 produtos mais valiosos", "Categoria mais valiosa", "Numero de produtos", "Voltar"};
    char item[30];
    starty = (LINES - HEIGHT) / 2;
    startx = (COLS - width) / 2;
    keypad(win, TRUE);
    noecho();
    switch(nchoice) {
    case 0:
        i = 0;
        werase(win);
        wrefresh(win);
        Porcategoria(catalog, win);
        wclear(win);
        win = create_newwin(24, width, starty, startx);
        keypad(win, TRUE);
        for(j = 0; j < 4; j++) {
            if(j == 0)
                wattron(win, A_STANDOUT);
            else
                wattroff(win, A_STANDOUT);
            sprintf(item, "%-7s",  listagem[j]);
            mvwprintw(win, j + 1, 2, "%s", item);
            wrefresh(win);
        }

    for( j=0; j<4; j++ ) {
         if( j == 0 )
             wattron( win, A_STANDOUT );
         else
             wattroff( win, A_STANDOUT );
         sprintf(item, "%-7s",  listagem[j]);
         mvwprintw( win, j+1, 2, "%s", item );
     }
     wrefresh(win);
     i = 0;
     exit = 1;
     while((key = wgetch(win)) != 'q' && exit == 1 ) {
         sprintf(item, "%-7s", listagem[i]);
         mvwprintw(win, i+1, 2, "%s", item);
         switch( key ) {
         case KEY_UP:
             i = upKey(win, 4, i);
             break;
         case KEY_DOWN:
             i = downKey(win, 4, i);
             break;
         case '\n':
             switch(i) {
             case 0://inserir lista por ordem alfabetica
                 break;
             case 1: //inserir lista por preco
                 wclear(win);
                 win = create_newwin(24, width, starty, startx);
                 keypad(win, TRUE);
                 for( j=0; j<4; j++ ) {
                     if( j == 0 )
                         wattron( win, A_STANDOUT );
                     else
                         wattroff( win, A_STANDOUT );
                     sprintf(item, "%-7s",  listagem[j]);
                     mvwprintw( win, j+1, 2, "%s", item );
                     wrefresh(win);
                 }
                 break;
             case 2:
                 i=0;
                 werase(win);
                 wrefresh(win);
                 Porcategoria(catalog,win);
                 wclear(win);
                 win = create_newwin(24, width, starty, startx);
                 keypad(win, TRUE);
                 for( j=0; j<4; j++ ) {
                     if( j == 0 )
                         wattron( win, A_STANDOUT );
                     else
                         wattroff( win, A_STANDOUT );
                     sprintf(item, "%-7s",  listagem[j]);
                     mvwprintw( win, j+1, 2, "%s", item );
                     wrefresh(win);
                 }

                 break;
             case 3:
                 mvwaddstr(win, 5, 2,"Presse ENTER para continuar");
                 wrefresh(win);
                 exit = 0;
                 break;
             }
         }
         wattron(win, A_STANDOUT);
         sprintf(item, "%-7s", listagem[i]);
         mvwprintw(win, i+1, 2, "%s", item);
         wattroff(win, A_STANDOUT);
         wrefresh(win);
         if(i == 3 && key == '\n') {
             sprintf(item, "%-7s", listagem[i]);
             mvwprintw(win, i+1, 2, "%s", item);
         }
     }
     wrefresh(win);
     break;
    case 1:
        endwin();






        displayData(catalog);
        awaitNewline(2);

        for( j=0; j<4; j++ ) {
            if( j == 0 )
                wattron( win, A_STANDOUT );
            else
                wattroff( win, A_STANDOUT );
            sprintf(item, "%-7s",  dados[j]);
            mvwprintw( win, j+1, 2, "%s", item );
        }
        wrefresh(win);
        i = 0;
        exit=1;
        while((key = wgetch(win)) != 'q' && exit == 1) {
            sprintf(item, "%-7s", dados[i]);
            mvwprintw(win, i+1, 2, "%s", item);
            switch( key ) {
            case KEY_UP:
                i = upKey(win, 4, i);
                break;
            case KEY_DOWN:
                i = downKey(win, 4, i);
                break;
            case '\n':




                switch(i) {
                case 0://apresentar 3 produtos mais valiosos
                    break;
                case 1: //apresentar categoria mais valiosa
                    break;
                case 2:
                    break;//apresentar numero de produtos
                case 3:
                    mvwaddstr(win, 5, 2,"Presse ENTER para continuar");
                    wrefresh(win);
                    exit = 0;
                    break;
                }
                break;

            }
            wattron(win, A_STANDOUT);
            sprintf(item, "%-7s", dados[i]);
            mvwprintw(win, i+1, 2, "%s", item);
            wattroff(win, A_STANDOUT);
            wrefresh(win);
            if(i == 3 && key == '\n') {
                sprintf(item, "%-7s", dados[i]);
                mvwprintw(win, i+1, 2, "%s", item);
            }
        }

        wrefresh(win);
        exit = 0;
        break;
    case 2:
        *ch = 'q';
        break;
    }
}

int upKey(WINDOW * win, int noptions, int i) {
    i--;
    i = (i < 0) ? noptions - 1 : i;
    return i;
}

int downKey(WINDOW * win, int noptions, int i) {
    i++;
    i = (i > noptions - 1) ? 0 : i;
    return i;
}

char * getMenuItem(Catalog * catalog, int i) {
    if(i == catCategoryCount(catalog)) return "Voltar";
    return catCategoryName(catalog, i);
}

void Porcategoria(Catalog * catalog, WINDOW * win) {
    char item[40];
    int i, skey, exit, categoryCount;
    categoryCount = catCategoryCount(catalog);
    exit = 1;


    initscr();


    wclear(win);
    wrefresh(win);
    win = create_newwin(24, 50, 0, 0);
    wrefresh(win);
    for(i = 0; i <= categoryCount ; ++i) {
        if(i == 0)
            wattron(win, A_STANDOUT);
        else
            wattroff(win, A_STANDOUT);
        sprintf(item, "%-7s",  getMenuItem(catalog, i));
        mvwprintw(win, i + 1, 2, "%s", item);
    }



    wrefresh(win);


    noecho();
    keypad(win, TRUE);
    i = 0;
    while((skey = wgetch(win)) != 'q' && exit) {
        sprintf(item, "%-7s",  getMenuItem(catalog, i));
        mvwprintw(win, i + 1, 2, "%s", item);
        switch(skey) {
        case KEY_UP:
            i = upKey(win, categoryCount + 1, i);
            break;
        case KEY_DOWN:
            i = downKey(win, categoryCount + 1, i);
            break;
        case '\n':
            if(i == categoryCount) {
                skey = 'q';
                exit = 0;
                mvwaddstr(win, categoryCount + 2, 2, "Presse ENTER para continuar");
                break;
            }
            endwin();
            ListingConfig config;
            config.useFilter = true;
            config.orderAlphabetical = true;
            config.categoryFilter = i;
            interactListing(catalog, &config);
            skey = 'q';
            wrefresh(win);
            break;

        }
        wattron(win, A_STANDOUT);
        sprintf(item, "%-7s",  getMenuItem(catalog, i));
        mvwprintw(win, i + 1, 2, "%s", item);
        wattroff(win, A_STANDOUT);
    }
    wclear(win);
    wrefresh(win);
}

*/

