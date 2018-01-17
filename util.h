#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PRODUCT_ID_LENGTH 20
#define MAX_NAME_LENGTH 40
#define FORMAT_NAME "%-40s"
#define MAX_CATEGORIES 100
#define MAX_STRING_LENGTH 200

// General utilities.

int imax(int a, int b);
int imin(int a, int b);
void awaitNewline(size_t n);
#define loop for(;;)
typedef char * Filepath;
void unimplemented(void); // try to alert at runtime when something isn't implemented
void * tryDereference(void **);

// Comparisons. A <= B?
bool lexiographicCompare(void * ptrA, void * ptrB); // Pointers to char.
bool priceCompare(void * dataA, void * dataB); // Pointers to Price.

// Pseudorandomness helpers.
void initRandomSeed();
int randomIntRange(int low, int high);
void randomWordFixed(size_t length, char *);
void randomName(char *);

// Strings.
void fillString(char * dest, char * src, size_t length);

#endif // UTIL_H_INCLUDED
