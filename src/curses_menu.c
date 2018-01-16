#include "util.h"
#include "curses_menu.h"
#include <curses.h>

typedef struct CursesState {
    WINDOW *window;
} CursesState;

CursesState * newCursesState(void) {
}

void freeCursesState(CursesState * state) {
    delwin(state->window);
    endwin();
}

int interactMenu(char ** options, size_t optionCount) {
    for( i=0; i<3; i++ ) {
        if( i == 0 )
            wattron( win, A_STANDOUT );
        else
            wattroff( win, A_STANDOUT );
        sprintf(item, "%-7s",  menu[i]);
        mvwprintw( win, i+1, 2, "%s", item );
    }
    mvwaddstr(win, 5, 2,"Presse ENTER para continuar");
    wrefresh(win);
    i = 0;
    noecho();
    keypad(win, TRUE);
    curs_set(0);
    while((ch = wgetch(win)) != 'q') {
        curs_set(0);
        sprintf(item, "%-7s",  menu[i]);
        mvwprintw( win, i+1, 2, "%s", item );
        switch(ch) {
        case '\n':
            werase(win);
            win = create_newwin(7, width, starty, startx);
            wrefresh(win);
            enterKey(win, i, &ch);
            werase(win);

            box(win, 0, 0);
            for( j=0; j<3; j++ ) {
                if( j == 0 )
                    wattron( win, A_STANDOUT );
                else
                    wattroff( win, A_STANDOUT );
                sprintf(item, "%-7s",  menu[j]);
                mvwprintw( win, j+1, 2, "%s", item );
                wrefresh(win);
            }
            i=0;
            break;
        case KEY_UP:
            i = upKey(win, 3, i);
            break;
        case KEY_DOWN:
            i = downKey(win, 3, i);
            break;
        }
        wattron( win, A_STANDOUT );
        sprintf(item, "%-7s",  menu[i]);
        mvwprintw( win, i+1, 2, "%s", item);
        wattroff( win, A_STANDOUT );
        mvwaddstr(win, 5, 2,"Presse ENTER para continuar");
        if(ch == 'q') break;
    }
}
