#include "mem.h"

void* recalloc(void* block, size_t oldSize, size_t newSize){
    void* ptr = realloc(block, newSize);
    if (ptr == NULL) {
        //free(block); // ne nuzhen
        return NULL;
    }
    if (newSize > oldSize) memset((char*)ptr + oldSize, 0, newSize - oldSize);
    return ptr;
}