#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global_variable.h"

Variable *global_variable_table;

int global_variable_count = 0;
int global_variable_capacity;

void init_global_variable(void) {
    global_variable_count = 0;
    global_variable_capacity = 8;
    global_variable_table = malloc(sizeof(Variable) * 8);

    if (global_variable_table == NULL) {
        fprintf(stderr, "Runtime Error: Failed to allocate global variable table\n");
        exit(1);
    }
}

static void make_bigger(void) {
    int new_capacity = global_variable_capacity * 2;
    Variable *new_global_variable_table = realloc(global_variable_table, sizeof(Variable) * new_capacity);

    if (new_global_variable_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize global variable table\n");
        exit(1);
    }

    global_variable_capacity = new_capacity;
    global_variable_table = new_global_variable_table;
}

void free_all_global_variable(void) {
    for (int i = 0; i < global_variable_count; i++) {
        free(global_variable_table[i].name);
        global_variable_table[i].name = NULL;
    }

    free(global_variable_table);
    global_variable_table = NULL;
    global_variable_count = 0;
    global_variable_capacity = 0;
}


GlobalVariablesInfo find_global_variable(char *name) {
    GlobalVariablesInfo var;
    var.found = false;
    var.address = -1;
    var.depth = 0;
    for (int i = 0; i < global_variable_count; i++) {
        if (strcmp(global_variable_table[i].name, name) == 0) {
            var.address = global_variable_table[i].memory_index;
            var.type = global_variable_table[i].type;
            var.found = true;
            return var;
        }
    }
    
    return var;
}

int insert_global_variable(char *name, TypeKind* type, int len) {
    if (len < 0) {
        fprintf(stderr, "Runtime Error: Invalid variable name length\n");
        exit(1);
    }

    while (global_variable_count >= global_variable_capacity) {
        make_bigger();
    }
    
    int current_idx = global_variable_count;

    char *new_name = malloc(len + 1);
    if (new_name == NULL) {
        fprintf(stderr, "Runtime Error: Failed to allocate variable name\n");
        exit(1);
    }

    global_variable_table[current_idx].name = new_name;

    strcpy(global_variable_table[current_idx].name, name);

    global_variable_table[current_idx].type = *type;
    global_variable_table[current_idx].memory_index = global_variable_count;

    global_variable_count++;

    return global_variable_table[current_idx].memory_index;
}
