/**
 * Data record types and parsing / file IO routines.
 */
#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED
#include <stdio.h>
#include "util.h"

typedef char *Filepath;
#define PRODUCT_ID_LENGTH 20
#define MAX_STRING_LENGTH 100

typedef char ProductID[PRODUCT_ID_LENGTH];

typedef unsigned long int Price;

typedef struct {
    unsigned char code;
    char name[MAX_STRING_LENGTH];
} Category;

typedef struct {
    char name[MAX_STRING_LENGTH];
    Price price;
    Category *category;
    unsigned int instances;
} ProductRecord;

/// Top-level. ///

typedef enum {
    READ_OK = 0,
    READ_BAD_IO, // Can't write to the file.
    READ_BAD_PARSE, // Can't parse what's on the file.
    READ_BAD_SANITIZE // The parsed information can't be cataloged.
} ReadStatus;

ReadStatus loadFlatfile(
    Filepath filepath, void *reader,
    bool (*onDefCategory)(Category const*, void *),
    bool (*onDefRecord)(ProductRecord const*, void *));

ReadStatus loadDemoFlatfile(
    void *catcher,
    bool (*onDefCategory)(Category const*, void *),
    bool (*onDefRecord)(ProductRecord const*, void *));

typedef enum {
    WRITE_OK = 0,
    WRITE_BAD_PATH
} WriteStatus;

WriteStatus writeFlatfile(
    Filepath filepath, void *write,
    Category *(popCategory)(),
    ProductRecord *(popRecord)());

WriteStatus writeDemoFlatfile(Filepath filepath);

#endif // DATA_H_INCLUDED
