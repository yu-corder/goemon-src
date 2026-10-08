#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

Funcion *function_table;
FuncionParams *function_params_table;

int function_table_capacity;
int function_count_capacity;

int function_params_table_capacity;
int function_params_function_capacity;

void init_function_table(void) {
    function_table_capacity = 8;
    function_count_capacity = 64;
    function_table = malloc(sizeof(Funcion) * 8);

    for (int i = 0; i < function_table_capacity; i++) {
        function_table[i].address = malloc(sizeof(int) * 64);
        function_table[i].type = malloc(sizeof(TypeKind) * 64);
        function_table[i].name = malloc(sizeof(char *) * 64);
        function_table[i].params = malloc(sizeof(Params) * 64);
    }
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

void free_all_function_table(void) {
    for (int i = 0; i < function_table_capacity; i++) {
        for (int j = 0; j < function_table[i].function_count; j++) {
            free(function_table[i].name[j]);

            // free(function_table[i].param_name[j]);
            // free(function_table[i].param_len[j]);
            // free(function_table[i].param_type[j]);
            
        }
        free(function_table[i].address);
        free(function_table[i].type);
        free(function_table[i].name);

        // free(function_table[i].param_name);
        // free(function_table[i].param_len);
        // free(function_table[i].param_type);
        // free(function_table[i].param_count);
        // function_table[i].function_count = 0;
    }

    free(function_table);
}

FuncionParamsInfo find_function_params(char *name, int depth) {
    FuncionParamsInfo var;
    var.found = false;
    var.address = -1;

    for (int i = depth + 1; i >= 0; i--) {
        for (int j = 0; j < function_table[i].function_count; j++) {
            if (strcmp(function_table[i].name[j], name) == 0) {
                var.found = true;
                var.depth = i;
                var.param_count = function_table[i].params[j].param_count;
                var.params = function_table[i].params[j].name;
                var.type = function_table[i].params[j].type;
                var.len = function_table[i].params[j].len;
                return var;
            }
        }
    }

    return var;
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

void insert_function(char *name, Node *params, int address, int depth, TypeKind type, int len) {
    if (depth == function_table_capacity) make_function_bigger();

    int current_idx = function_table[depth].function_count;

    if (current_idx == function_count_capacity) {
        int new_capacity = function_count_capacity * 2;

        function_table[depth].address = realloc(
            function_table[depth].address,
            sizeof(int) * new_capacity
        );

        function_table[depth].type = realloc(
            function_table[depth].type,
            sizeof(TypeKind) * new_capacity
        );

        function_table[depth].name = realloc(
            function_table[depth].name,
            sizeof(char *) * new_capacity
        );

        function_table[depth].params = realloc(
            function_table[depth].params,
            sizeof(Params) * new_capacity
        );

        function_count_capacity = new_capacity;
    }
    

    for (int i = 0; i < function_table[depth].function_count; i++) {
        if (strcmp(function_table[depth].name[i], name) == 0) {
            return;
        }
    }

    function_table[depth].name[current_idx] = malloc(len + 1);
    strcpy(function_table[depth].name[current_idx], name);

    function_table[depth].address[current_idx] = address;
    function_table[depth].function_count++;
    function_table[depth].type[current_idx] = type;

    Node *p_tmp = params;
    Node *p = params;
    int param_count = 0;
    while (p_tmp) {
        if (p_tmp->lhs == NULL) {
            fprintf(stderr, "Parameter '%s' requires an explicit type declaration.\n", p_tmp->name);
            exit(1);
        }
        param_count++;
        p_tmp = p_tmp->next;
    }

    function_table[depth].params[current_idx].name = malloc(sizeof(char *) * param_count);
    function_table[depth].params[current_idx].type = malloc(sizeof(TypeKind) * param_count);
    function_table[depth].params[current_idx].len = malloc(sizeof(int) * param_count);
    function_table[depth].params[current_idx].param_count = param_count;

    param_count = 0;
    while (p) {
        function_table[depth].params[current_idx].len[param_count] = p->lhs->len;
        function_table[depth].params[current_idx].type[param_count] = p->type;

        function_table[depth].params[current_idx].name[param_count] = malloc(p->lhs->len + 1);
        strcpy(function_table[depth].params[current_idx].name[param_count], p->lhs->name);
        param_count++;
        p = p->next;
    }

}
