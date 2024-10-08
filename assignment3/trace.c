//trace file logic implementation

#include "trace.h"
#include <stdio.h>

void read_trace(Cache *cache) {
    char operation;
    unsigned int address;
    int size;

    while (scanf(" %c 0x%x %d", &operation, &address, &size) == 3) {
        printf("Operation: %c, Address: 0x%x, Size: %d\n", operation, address, size);
        //add cache access logic later
    }
}