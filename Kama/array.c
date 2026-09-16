#include "array.h"
#include <stdlib.h>

int array_count = 0;
Array array_table[128];

int make_array(int size) {
    int *array;
    int index = array_count;
    array = malloc(sizeof(int) * size);

    array_table[index].length = size;
    array_table[index].elements = array;
    
    array_count++;
    return index;
}

void store_array(int heap_index, int index, int value) {
    array_table[heap_index].elements[index] = value;
}

int get_data(int index, int addr) {
    return array_table[index].elements[addr];
}