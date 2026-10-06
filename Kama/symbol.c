#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

Funcion *function_table;
FuncionParams *function_params_table;

int function_table_capacity;

int function_params_table_capacity;

int function_params_function_capacity;

void init_function_table(void) {
    function_table_capacity = 8;
    function_table = malloc(sizeof(Funcion) * 8);
}

static void make_function_bigger(void) {
    int new_capacity = function_table_capacity * 2;
    Funcion *new_function_table = realloc(function_table, sizeof(Funcion) * new_capacity);

    if (new_function_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize function table\n");
        exit(1);
    }

    function_table_capacity = new_capacity;
    function_table = new_function_table;
}

void init_function_params_table(void) {
    function_params_table_capacity = 8;
    function_params_table = malloc(sizeof(FuncionParams) * 8);
    function_params_function_capacity = 16;

    for (int i = 0; i < function_params_table_capacity; i++) {
        function_params_table[i].param_count = malloc(sizeof(int) * 16);
    }
}

static void make_function_params_bigger(void) {
    int new_capacity = function_params_table_capacity * 2;
    FuncionParams *new_function_params_table = realloc(function_params_table, sizeof(FuncionParams) * new_capacity);

    if (new_function_params_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize function table\n");
        exit(1);
    }

    function_params_table_capacity = new_capacity;
    function_params_table = new_function_params_table;
}

FuncionParamsInfo find_function_params(char *name, int depth) {
    FuncionParamsInfo var;
    var.found = false;
    var.address = -1;

    for (int i = depth + 1; i >= 0; i--) {
        for (int j = 0; j < function_params_table[i].function_count; j++) {
            if (strcmp(function_params_table[i].name[j], name) == 0) {
                var.found = true;
                var.depth = i;
                var.param_count = function_params_table[i].param_count[j];
                var.params = function_params_table[i].params[j];
                var.type = function_params_table[i].type[j];
                var.len = function_params_table[i].len[j];
                return var;
            }
        }
    }

    return var;
}

void insert_function_params(char *name, Node *params, int depth) {
    if (depth == function_params_table_capacity) make_function_params_bigger();
    int current_idx = function_params_table[depth].function_count;

    if (current_idx == function_params_function_capacity) {
        int new_capacity = function_params_function_capacity * 2;

        function_params_table[depth].param_count = realloc(
            function_params_table[depth].param_count,
            sizeof(int) * new_capacity
        );
        function_params_function_capacity = new_capacity;
    }
    

    for (int i = 0; i < function_params_table[depth].function_count; i++) {
        if (strcmp(function_params_table[depth].name[i], name) == 0) {
            return;
        }
    }

    strcpy(function_params_table[depth].name[current_idx], name);

    Node *p = params;

    int p_count = 0;
    while (p) {
        if (p->lhs == NULL) {
            fprintf(stderr, "Parameter '%s' requires an explicit type declaration.\n", p->name);
            exit(1);
        }
        strcpy(function_params_table[depth].params[current_idx][p_count], p->lhs->name);
        function_params_table[depth].type[current_idx][p_count] = p->type;
        function_params_table[depth].len[current_idx][p_count] = p->lhs->len;
        p_count++;
        p = p->next;
    }
    
    function_params_table[depth].param_count[current_idx] = p_count;
    function_params_table[depth].function_count++;
}


FuncionInfo find_function(char *name, int depth) {
    FuncionInfo var;
    var.found = false;
    var.address = -1;

    for (int i = depth + 1; i >= 0; i--) {
        for (int j = 0; j < function_table[i].function_count; j++) {
            if (strcmp(function_table[i].name[j], name) == 0) {
                var.found = true;
                var.address = function_table[i].address[j];
                var.depth = i;
                var.type = function_table[i].type[j];
                return var;
            }
        }
    }

    return var;
}

void insert_function(char *name, int address, int depth, TypeKind type) {
    if (depth == function_table_capacity) make_function_bigger();

    int current_idx = function_table[depth].function_count;
    

    for (int i = 0; i < function_table[depth].function_count; i++) {
        if (strcmp(function_table[depth].name[i], name) == 0) {
            return;
        }
    }

    strcpy(function_table[depth].name[current_idx], name);
    function_table[depth].address[current_idx] = address;
    function_table[depth].function_count++;
    function_table[depth].type[current_idx] = type;
}
