#include "menu.h"
#include <curses.h>

#define HEIGHT 20
#define WIDTH 60

ScrollState initialScrollState(void) {
    ScrollState state;
    state.scroll = 0;
    state.cursor = 0;
    return state;
}

typedef struct CursesState {
    WINDOW * win;
    int lines, cols;
    int startX, startY;
    char commandBuffer[MAX_STRING_LENGTH];
} CursesState;

CursesState * openCurses(void) {
    CursesState * state = malloc(sizeof(CursesState));
    strcpy(state->commandBuffer, "hello world");
    initscr();
    state->startY = 0; // (LINES - HEIGHT) / 2;
    state->startX = 0; // (COLS - WIDTH) / 2;
    state->lines = LINES - 2;
    state->cols = COLS - 2;
    refresh();
    state->win = newwin(state->lines + 2, state->cols + 2, state->startX, state->startY);
    noecho();
    keypad(state->win, TRUE);
    curs_set(0);
    return state;
}

void closeCurses(CursesState * state) {
    delwin(state->win);
    endwin();
    free(state);
}


void getDisplayLine(
    void * userData, CursesState * state,
    void (*getLine)(void *, size_t, char *),
    size_t lineCount, size_t line, char * lineBuffer) {
    if(line < 0 || line > lineCount - 1) {
        fillString(lineBuffer, "", state->cols);
        return;
    }

    char userLine[MAX_STRING_LENGTH];
    getLine(userData, line, userLine);
    fillString(lineBuffer, userLine, state->cols);
}

void scrollScreen(CursesState * curses, ScrollState *state, size_t lineCount, int delta) {
    int target = imin(lineCount - 1, imax(0, state->scroll + state->cursor + delta));
    delta = target - (state->cursor + state->scroll);
    if (delta + state->cursor > curses->cols - 1) state->cursor = curses->cols - 1;
    else if (delta + state->cursor < 0) state->cursor = 0;
    else state->cursor = target - state->scroll;
    state->scroll = target - state->cursor;
}

InteractResult interactVirtual(void * userData,
                               CursesState * curses,
                               void (*getLine)(void *, size_t, char *),
                               size_t lineCount, ScrollState state) {
    bool commandMode = false;
    size_t commandLength = 0;
    loop {
        // Draw everything.
        // box(state->win, 0, 0);
        for(size_t line = 0; line < curses->lines; line++) {
            if(line == state->cursor)
                wattron(curses->win, A_STANDOUT);
            else
                wattroff(curses->win, A_STANDOUT);

            char lineBuffer[curses->cols + 1];
            getDisplayLine(userData, curses, getLine, lineCount, line + scroll, lineBuffer);
            mvwprintw(curses->win, line + 1, 1, "%s", lineBuffer);
            wattroff(curses->win, A_STANDOUT);
        }
        if(commandMode) {
            char lineBuffer[curses->cols];
            fillString(lineBuffer, curses->commandBuffer, curses->cols - 1);
            mvwprintw(curses->win, curses->lines + 1, 1, "/%s", lineBuffer);
        } else {
            char lineBuffer[curses->cols + 1];
            fillString(lineBuffer, "", curses->cols);
            mvwprintw(curses->win, curses->lines + 1, 1, "%s", lineBuffer);
        }
        wrefresh(curses->win);

        // Interpret user input.
        InteractResult result;
        int ch = wgetch(curses->win);
        if(commandMode) {
            if(ch == KEY_BACKSPACE && commandLength > 0) --commandLength;
            if(ch == ' ' || ('a' <= ch && 'z' >= ch)) state->commandBuffer[commandLength++] = ch;
            if(ch == 27) {
                commandMode = false;
                commandLength = 0;
            }
            if(ch == '\n') {
                InteractResult result;
                result.hasCommand = true;
                result.command = &state->commandBuffer;
                result.state = state;
                result.option = state->cursor + state->scroll;
                return result;
            }
            state->commandBuffer[commandLength] = '\0';
            continue;
        }
        switch(ch) {
        case '\n':
            result.hasCommand = false;
            result.state = state;
            result.option = state->scroll + state->cursor;
            return result;
        case '/':
            commandMode = true;
            commandLength = 0;
            break;
        case KEY_UP:
            scrollScreen(state, &cursor, &scroll, lineCount, -1);
            break;
        case KEY_DOWN:
            scrollScreen(state, &cursor, &scroll, lineCount, 1);
            break;
        case KEY_NPAGE:
            scrollScreen(state, &cursor, &scroll, lineCount, curses->cols - 1);
            break;
        case KEY_PPAGE:
            scrollScreen(state, &cursor, &scroll, lineCount, 1 - curses->cols);
            break;
        case KEY_HOME:
            cursor = 0;
            scroll = 0;
            break;
        case KEY_END:
            cursor = 0;
            scroll = 0;
            scrollScreen(state, &cursor, &scroll, lineCount, lineCount - 1);
            break;
        default:
            break;
        }
    }
}

void concreteGetLine(void * ptr, size_t line, char * buffer) {
    strcpy(buffer, (char *)((char(*) [MAX_STRING_LENGTH]) ptr)[line]);  // oh gosh oh jeez
}

InteractResult interact(CursesState * state, char lines[][MAX_STRING_LENGTH], size_t lineCount, size_t initialLine) {
    return interactVirtual((void *) lines, state, &concreteGetLine, lineCount, initialLine);
}
