#ifndef ARRAY_H
#define ARRAY_H


typedef struct {
    int length;
    int *elements;
} Array;

int make_array(int size);
int get_data(int index, int addr);
void store_array(int heap_index, int index, int value);
int load_array(int heap_index, int index);
#endif
