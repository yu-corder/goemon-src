#ifndef ARRAY_H
#define ARRAY_H

#include "type.h"

typedef struct {
    int length;
    TypeKind type;
    int *elements;
} Array;

int make_array(int size, TypeKind type);
int get_data(int index, int addr);
void store_array(int heap_index, int index, int value);
int load_array(int heap_index, int index);
#endif
