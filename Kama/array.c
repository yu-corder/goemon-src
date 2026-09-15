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

int get_data(int index, int addr) {
    return array_table[index].elements[addr];
}