#ifndef ARRAY_H
#define ARRAY_H


typedef struct {
    int length;
    int *elements;
} Array;

int make_array(int size);
int get_data(int index, int addr);
#endif
