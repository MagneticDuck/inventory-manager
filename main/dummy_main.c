#include "interact.h"
#include "util.h"

int main(void) {
    CursesState * curses = openCurses();
    char list[200][MAX_STRING_LENGTH];
    for (size_t i = 0; i < 200; ++i) sprintf(list[i], "item %lu", i);

    ScrollState state = initialScrollState();

    loop {
        InteractResult result = interact(curses, list, 200, state);
        state = result.state;
        if (result.hasCommand)
            strcpy(list[result.option], result.command);
    }

    closeCurses(curses);
}
