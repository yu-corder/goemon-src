#include "array.h"

int array_count = 0;
Array array_table[128];

int make_array(int size) {
    int index = array_count;
    int nums[size];
    array_table[index].elements = nums;
    for(int i = 0; i < size; i++) {
        array_table[index].elements[i] = 5;
    }
    array_count++;
    return index;
}

int get_data(int index, int addr) {
    return array_table[index].elements[addr];
}