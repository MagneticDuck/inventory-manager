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

void unimplemented(void);

// A <= B
bool lexiographicCompare(void * ptrA, void * ptrB);
bool numericCompare(void * dataA, void * dataB);

// Random.
void initRandomSeed();
int randomIntRange(int low, int high);
void randomWordFixed(size_t length, char *);
void randomWord(char *);

void getRandomName(char str[]);

timer_t getTime();

#endif // UTIL_H_INCLUDED
