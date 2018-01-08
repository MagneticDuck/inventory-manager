#include "util.h"

void unimplemented(void) {
    printf("asjdfjslkdjflkdsa");
}

bool lexiographicCompare(void * ptrA, void * ptrB) {
    char * strA = (char *) ptrA, * strB = (char *) ptrB;
    for(size_t i = 0;; ++i) {
        if(!strA[i]) return 1;
        if(!strB[i]) return 0;
        if(strA[i] != strB[i]) return strA[i] <= strB[i];
    }
}

bool numericCompare(void* dataA, void* dataB) {
    return (int *) dataA <= (int *) dataB;
}

void getRandomName(char name[]) {

}
