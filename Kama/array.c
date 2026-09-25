#include "array.h"
#include <stdlib.h>

int array_count = 0;
Array *array_table;

void init_array_table(void) {
    array_table = malloc(sizeof(Array) * 128);
}

int make_array(int size, TypeKind type) {
    int *array;
    int index = array_count;

    array = malloc(sizeof(int) * size);
    

    array_table[index].length = size;
    array_table[index].type = type;
    array_table[index].elements = array;
    
    array_count++;
    return index;
}

void store_array(int array_index, int index, int value) {
    array_table[array_index].elements[index] = value;
}

int load_array(int array_index, int index) {
    return array_table[array_index].elements[index];
}

void free_all_arrays(void) {
    for (int i = 0; i < array_count; i++) {
        free(array_table[i].elements);
        array_table[i].elements = NULL;
    }

    free(array_table);
    array_table = NULL;
    array_count = 0;
}