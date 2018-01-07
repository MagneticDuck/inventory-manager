#include<stdio.h>
#include<stdlib.h>
#include<curses.h>
WINDOW *create_newwin(int height, int width, int starty, int startx);
void enterKey(WINDOW *win, int nchoice, int *ch);
int upKey(WINDOW *win, int noptions, int i);
int downKey(WINDOW *win, int noptions, int i);


int main(){
    WINDOW *win;
    int startx, starty, width, height;
    int ch, i = 0;
    char item[30];
    char menu[3][9]= {"Listagem", "Dados", "Sair"};

    initscr();
    //cbreak();


    height = 5;
    width = 40;

    starty = (LINES - height) / 2;
    startx = (COLS - width) / 2;
    refresh();
    win = create_newwin(height, width, starty, startx);
    for( i=0; i<3; i++ ) {
            if( i == 0 )
                wattron( win, A_STANDOUT );
            else
                wattroff( win, A_STANDOUT );
                sprintf(item, "%-7s",  menu[i]);
                mvwprintw( win, i+1, 2, "%s", item );
    }
    wrefresh(win);
    i = 0;
    noecho();
    keypad(win, TRUE);
    curs_set(0);
    while((ch = wgetch(win)) != 'q'){
        sprintf(item, "%-7s",  menu[i]);
        mvwprintw( win, i+1, 2, "%s", item );
        switch(ch){
            case '\n':
                werase(win);
                win = create_newwin(height, width, starty, startx);
                wrefresh(win);
                enterKey(win, i, &ch);
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

    if(ch == 'q') break;

    }

    delwin(win);
    endwin();

}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}


void enterKey(WINDOW *win, int nchoice, int *ch){
    int j, key, i;
    char listagem[2][30] = {"Por ordem alfabetica", "Por Preco"};
    char dados[3][30] = {"3 produtos mais valiosos", "categoria mais valiosa", "numero de produtos"};
    char item[30];
    keypad(win, TRUE);
    noecho();
    switch(nchoice){
        case 0:
           for( j=0; j<2; j++ ) {
                        if( j == 0 )
                            wattron( win, A_STANDOUT );
                        else
                            wattroff( win, A_STANDOUT );
                            sprintf(item, "%-7s",  listagem[j]);
                            mvwprintw( win, j+1, 2, "%s", item );
            }
            wrefresh(win);
            i = 0;
            while((key = wgetch(win)) != 'q'){
                sprintf(item, "%-7s", listagem[i]);
                mvwprintw(win, i+1, 2, "%s", item);
                switch( key ){
                    case KEY_UP:
                        i = upKey(win, 2, i);
                        break;
                    case KEY_DOWN:
                        i = downKey(win, 2, i);
                        break;
                }
                wattron(win, A_STANDOUT);
                sprintf(item, "%-7s", listagem[i]);
                mvwprintw(win, i+1, 2, "%s", item);
                wattroff(win, A_STANDOUT);
                wrefresh(win);
            }
            wgetch(win);
            wrefresh(win);
            break;
        case 1:
            for( j=0; j<3; j++ ) {
                        if( j == 0 )
                            wattron( win, A_STANDOUT );
                        else
                            wattroff( win, A_STANDOUT );
                            sprintf(item, "%-7s",  dados[j]);
                            mvwprintw( win, j+1, 2, "%s", item );
            }
             wrefresh(win);
            i = 0;
            while((key = wgetch(win)) != 'q'){
                sprintf(item, "%-7s", dados[i]);
                mvwprintw(win, i+1, 2, "%s", item);
                switch( key ){
                    case KEY_UP:
                        i = upKey(win, 3, i);
                        break;
                    case KEY_DOWN:
                        i = downKey(win, 3, i);
                        break;
                }
                wattron(win, A_STANDOUT);
                sprintf(item, "%-7s", dados[i]);
                mvwprintw(win, i+1, 2, "%s", item);
                wattroff(win, A_STANDOUT);
                wrefresh(win);
            }
            wgetch(win);
            wrefresh(win);
            break;
        case 2:
            *ch = 'q';
            break;

    }

}

int upKey(WINDOW *win, int noptions, int i)
{
    i--;
    i = (i < 0)? noptions-1: i;
    return i;
}

int downKey(WINDOW *win, int noptions, int i)
{
    i++;
    i = (i > noptions -1)? 0: i;
    return i;
}

