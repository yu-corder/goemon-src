#include "array.h"
#include <stdlib.h>
#include <stdio.h>

int array_count = 0;
int array_capacity = 128;
Array *array_table;

void init_array_table(void) {
    array_count = 0;
    array_capacity = 128;
    array_table = malloc(sizeof(Array) * array_capacity);

    if (array_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to allocate array table\n");
        exit(1);
    }
}

static void make_bigger(void) {
    int new_capacity = array_capacity * 2;
    Array *new_array_table = realloc(array_table, sizeof(Array) * new_capacity);

    if (new_array_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize array table\n");
        exit(1);
    }

    array_capacity = new_capacity;
    array_table = new_array_table;
}

int make_array(int size, TypeKind type) {
    while (array_count >= array_capacity) {
        make_bigger();
    }

    if (size <= 0) {
        fprintf(stderr, "Runtime Error: Invalid array size\n");
        exit(1);
    }

    int index = array_count;

    int *new_elements = malloc(sizeof(int) * size);
    if (new_elements == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to allocate array\n");
        exit(1);
    }

    array_table[index].length = size;
    array_table[index].type = type;
    array_table[index].elements = new_elements;
    
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
    array_capacity = 0;
}