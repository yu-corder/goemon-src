#include "array.h"
#include <stdlib.h>

int array_count = 0;
Array array_table[128];

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

void store_array(int heap_index, int index, int value) {
    array_table[heap_index].elements[index] = value;
}

int load_array(int heap_index, int index) {
    return array_table[heap_index].elements[index];
}

int get_data(int index, int addr) {
    return array_table[index].elements[addr];
}

bool free_array_table() {
    for (int i = 0; i < array_count; i++) {
        free(array_table[i].elements);
        array_table[i].elements = NULL;
    }
    array_count = 0;
    return true;
}