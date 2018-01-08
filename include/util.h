/**
 * Utilities.
 */
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define loop for(;;)

// A <= B
bool lexiographicCompare(void * ptrA, void * ptrB);
bool numericCompare(void * dataA, void * dataB);

void getRandomName(char str[]);

timer_t getTime();

#endif // UTIL_H_INCLUDED
