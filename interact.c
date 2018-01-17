#include "interact.h"
#include "curses.h"

typedef struct CursesState {
    WINDOW * win;
    size_t lines, cols;
    size_t startX, startY;
    char commandBuffer[MAX_STRING_LENGTH];
} CursesState;

void scrollScreen(CursesState * curses, ScrollState * state, size_t lineCount, int delta) {
    if (lineCount == 0) return;
    int target = imin(lineCount - 1, imax(0, state->scroll + state->cursor + delta));
    delta = target - (state->cursor + state->scroll);
    if(delta + (int) state->cursor > (int) curses->lines - 1) state->cursor = curses->lines - 1;
    else if(delta + (int) state->cursor < 0) state->cursor = 0;
    else state->cursor = target - state->scroll;
    state->scroll = target - state->cursor;
}

ScrollState initialScrollState(void) {
    ScrollState state;
    state.scroll = 0;
    state.cursor = 0;
    return state;
}

ScrollState startScrollAt(CursesState * curses,
                          size_t line, size_t lineCount) {
    ScrollState state = initialScrollState();
    scrollScreen(curses, &state, lineCount, line);
    return state;
}

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
    raw();
    noecho();
    cbreak();
    keypad(state->win, TRUE);
    curs_set(0);
    return state;
}

void closeCurses(CursesState * state) {
    delwin(state->win);
    endwin();
    free(state);
    nocbreak();
}

void getDisplayLine(
    void * userData, CursesState * state,
    void (*getLine)(void *, size_t, char *),
    size_t lineCount, int line, char * lineBuffer) {
    if (lineCount == 0 && line == 0) {
        fillString(lineBuffer, "<empty>", state->cols);
        return;
    }
    if(line < 0 || line > (int) lineCount - 1) {
        fillString(lineBuffer, "", state->cols);
        return;
    }

    char userLine[MAX_STRING_LENGTH + 1];
    getLine(userData, line, userLine);
    fillString(lineBuffer, userLine, state->cols);
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
        if (state.scroll > 0) mvwprintw(curses->win, 0, 0, "^");
        else mvwprintw(curses->win, 0, 0, " ");
        if (state.scroll + curses->lines < lineCount) mvwprintw(curses->win, curses->lines + 1, 0, "v");
        else mvwprintw(curses->win, curses->lines + 1, 0, " ");
        for(size_t line = 0; line < curses->lines; line++) {
            if(line == state.cursor)
                wattron(curses->win, A_STANDOUT);
            else
                wattroff(curses->win, A_STANDOUT);

            char lineBuffer[curses->cols + 1];
            getDisplayLine(userData, curses, getLine, lineCount, (int) line + state.scroll, lineBuffer);
            mvwaddstr(curses->win, line + 1, 1, lineBuffer);
            wattroff(curses->win, A_STANDOUT);
        }
        if(commandMode) {
            char lineBuffer[curses->cols + 1];
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
#ifdef WINDOWS_IS_GREAT
            if(ch == '\r') {
#else
            if(ch == '\n')
#endif
                InteractResult result;
                result.hasCommand = true;
                curses->commandBuffer[commandLength] = '\0';
                result.command = curses->commandBuffer;
                result.state = state;
                result.option = state.cursor + state.scroll;
                return result;
            }
#ifdef WINDOWS_IS_GREAT
            if((ch == KEY_LEFT || ch == '\b') && commandLength > 0) --commandLength;
#else
            if((ch == KEY_LEFT || ch == KEY_BACKSPACE) && commandLength > 0) --commandLength;
#endif
            if(ch == ' ' || ('0' <= ch && '9' >= ch) || ('a' <= ch && 'z' >= ch))
                curses->commandBuffer[commandLength++] = ch;
            if(ch == 27) {
                commandMode = false;
                commandLength = 0;
            }
            strcpy(curses->commandBuffer + commandLength, "_");
            continue;
        }
        switch(ch) {
        case KEY_LEFT:
        case 'q':
            result.hasCommand = false;
            result.state = state;
            result.option = state.scroll + state.cursor;
            result.isQuit = true;
            return result;
        case KEY_RIGHT:
#ifdef WINDOWS_IS_GREAT
        case '\r':
#else
        case '\n':
#endif
            result.hasCommand = false;
            result.state = state;
            result.option = state.scroll + state.cursor;
            return result;
        case '/':
            commandMode = true;
            commandLength = 0;
            strcpy(curses->commandBuffer, "_");
            break;
        case KEY_UP:
            scrollScreen(curses, &state, lineCount, -1);
            break;
        case KEY_DOWN:
            scrollScreen(curses, &state, lineCount, 1);
            break;
        case KEY_NPAGE:
            scrollScreen(curses, &state, lineCount, curses->lines- 1);
            break;
        case KEY_PPAGE:
            scrollScreen(curses, &state, lineCount, 1 - curses->lines);
            break;
        case KEY_HOME:
            state = initialScrollState();
            break;
        case KEY_END:
            state = initialScrollState();
            scrollScreen(curses, &state, lineCount, lineCount - 1);
            break;
        default:
            break;
        }
    }
}

void concreteGetLine(void * ptr, size_t line, char * buffer) {
    strcpy(buffer, (char *)((char(*) [MAX_STRING_LENGTH]) ptr)[line]);  // oh gosh oh jeez
}

InteractResult interact(CursesState * curses, char lines[][MAX_STRING_LENGTH], size_t lineCount, ScrollState state) {
    return interactVirtual((void *) lines, curses, &concreteGetLine, lineCount, state);
}
