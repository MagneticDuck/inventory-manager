#ifndef MENU_INCLUDED
#define MENU_INCLUDED
#include "util.h"
#include "data.h"

typedef struct CursesState CursesState;

typedef struct {
    size_t scroll, cursor;
} ScrollState;

ScrollState initialScrollState(void);

typedef struct {
    ScrollState state;
    size_t option;
    bool hasCommand;
    char * command;
} InteractResult;

CursesState * openCurses(void);
void closeCurses(CursesState * state);

InteractResult interactVirtual(void * userData, CursesState * curses, void (*showLine)(void *, size_t, char *), size_t lineCount, ScrollState scroll);
InteractResult interact(CursesState * curses, char lines[][MAX_STRING_LENGTH], size_t lineCount, ScrollState scroll);

#endif // MENU_INCLUDED
