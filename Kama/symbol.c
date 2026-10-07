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
        }
        free(function_table[i].address);
        free(function_table[i].type);
        free(function_table[i].name);
        // function_table[i].function_count = 0;
    }

    free(function_table);
}

void init_function_params_table(void) {
    function_params_table_capacity = 8;
    function_params_table = malloc(sizeof(FuncionParams) * 8);
    function_params_function_capacity = 16;

    for (int i = 0; i < function_params_table_capacity; i++) {
        function_params_table[i].param_count = malloc(sizeof(int) * 16);
        function_params_table[i].len = malloc(sizeof(int *) * 16);
        function_params_table[i].type = malloc(sizeof(TypeKind *) * 16);
    }
}

void free_all_function_params(void) {
    for (int i = 0; i < function_params_table_capacity; i++) {
        for (int j = 0; j < function_params_table[i].function_count; j++) {
            free(function_params_table[i].len[j]);
            free(function_params_table[i].type[j]);
        }
        free(function_params_table[i].param_count);
        free(function_params_table[i].len);
        free(function_params_table[i].type);
        function_params_table[i].param_count = NULL;
    }

    free(function_params_table);
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

        function_params_table[depth].len = realloc(
            function_params_table[depth].len,
            sizeof(int *) * new_capacity
        );

        function_params_table[depth].type = realloc(
            function_params_table[depth].type,
            sizeof(TypeKind *) * new_capacity
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
    Node *p_tmp = params;
    int p_count = 0;
    while (p_tmp) {
        if (p->lhs == NULL) {
            fprintf(stderr, "Parameter '%s' requires an explicit type declaration.\n", p_tmp->name);
            exit(1);
        }
        strcpy(function_params_table[depth].params[current_idx][p_count], p_tmp->lhs->name);
        p_count++;
        p_tmp = p_tmp->next;
    }

    function_params_table[depth].type[current_idx] = malloc(sizeof(TypeKind) * p_count);
    function_params_table[depth].len[current_idx] = malloc(sizeof(int) * p_count);
    p_count = 0;
    while (p) {
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

void insert_function(char *name, int address, int depth, TypeKind type, int len) {
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
}
