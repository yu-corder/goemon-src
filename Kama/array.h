#ifndef ARRAY_H
#define ARRAY_H

#include "type.h"
#include <stdbool.h>

typedef struct {
    int length;
    TypeKind type;
    int *elements;
} Array;

int make_array(int size, TypeKind type);
void store_array(int heap_index, int index, int value);
int load_array(int heap_index, int index);
bool free_array_table();
#endif
