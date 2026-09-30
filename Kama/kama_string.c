#include <stdlib.h>
#include <stdio.h>
#include "kama_string.h"

String *string_table;
int string_count = 0;
int string_capacity;

void init_strings(void) {
    string_count = 0;
    string_capacity = 8;
    string_table = malloc(sizeof(String) * 8);
}

void make_bigger(void) {
    int new_capacity = string_capacity * 2;
    String *new_string_table = realloc(string_table, sizeof(String) * new_capacity);

    if (new_string_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize string\n");
        exit(1);
    }

    string_capacity = new_capacity;
    string_table = new_string_table;
}