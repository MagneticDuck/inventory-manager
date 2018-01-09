#include "array.h"

typedef struct Array {
    void ** elements;
    size_t size;
} Array;

Array * newArray(size_t maxSize) {
    Array * array = malloc(sizeof(Array));
    array->elements = malloc(sizeof(void *) * maxSize);
    array->size = 0;
    return array;
}

void freeArray(Array * array) {
    free(array->elements);
    free(array);
}

size_t arraySize(Array * array) {
    return array->size;
}

size_t arrayAppend(Array * array, void * data) {
    array->elements[array->size] = data;
    return array->size++;
}

void * arrayAccess(Array * array, size_t index) {
    return  array->elements[index];
}
