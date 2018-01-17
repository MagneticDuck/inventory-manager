int imax(int a, int b) {
    if (a > b) return a;
    return b;
}

int imin(int a, int b) {
    if (a > b) return b;
    return a;
}


#include <ncurses.h>
int getConsoleLines(void) {
    return LINES;
}

#include "util.h"
#include "data.h"

void awaitNewline(size_t n) {
    size_t x = 0;
    while(x < n) {
        x += (getchar() == '\n');
    }
}

void unimplemented(void) {
    printf("asjdfjslkdjflkdsa");
}

void * tryDereference(void ** ptr) {
    if(!ptr) return NULL;
    return *ptr;
}

bool lexiographicCompare(void * ptrA, void * ptrB) {
    char * strA = (char *) ptrA, * strB = (char *) ptrB;
    for(size_t i = 0;; ++i) {
        if(!strA[i]) return 1;
        if(!strB[i]) return 0;
        if(strA[i] != strB[i]) return strA[i] <= strB[i];
    }
}

bool priceCompare(void * ptrA, void * ptrB) {
    return *((Price *) ptrA) >= *((Price *) ptrB);
}

void initRandomSeed() {
    // srand(time(NULL));
}

int randomIntRange(int low, int high) {
    return (rand() % (low - high + 1)) + low;
}

void randomWordFixed(size_t length, char * str) {
    for(size_t i = 0; i < length; ++i)
        str[i] = (char) randomIntRange((int) 'a', (int) 'z');
    str[length] = '\0';
}

void randomWord(char * str) {
    randomWordFixed(randomIntRange(5, MAX_STRING_LENGTH), str);
}

bool numericCompare(void * dataA, void * dataB) {
    return (int *) dataA <= (int *) dataB;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
void fillString(char * dest, char * src, size_t length) {
    bool overflowing = false;
    for(size_t i = 0; i < length; ++i) {
      dest[i] = '_';
        overflowing = overflowing || src[i] == '\0';
        if(!overflowing) dest[i] = src[i];
        else dest[i] = '_';
    }
    dest[length] = '\0';
}
