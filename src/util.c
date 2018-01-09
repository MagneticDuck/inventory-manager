#include "util.h"
#include "data.h"

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

bool priceCompare(void * ptrA, void * ptrB) {
    return *((Price *) ptrA) <= *((Price *) ptrB);
}

void initRandomSeed() {
    srand(time(NULL));
}

int randomIntRange(int low, int high) {
    return (rand() % (low - high + 1)) + low;
}

void randomWordFixed(size_t length, char * str) {
    for (size_t i = 0; i < length; ++i)
        str[i] = (char) randomIntRange((int) 'a', (int) 'z');
    str[length] = '\0';
}

void randomWord(char * str) {
    randomWordFixed(rand() % 7 + 5, str);
}

bool numericCompare(void* dataA, void* dataB) {
    return (int *) dataA <= (int *) dataB;
}

void getRandomName(char name[]) {

}
