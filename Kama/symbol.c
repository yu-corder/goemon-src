#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

Label symbol_table[128];
Variable *global_variable_table;
LocalVariables *local_scopes;
Funcion *function_table;
FuncionParams *function_params_table;

int label_count_internal = 0;
int global_variable_count = 0;
int global_variable_capacity;

int local_variable_capacity;

int function_table_capacity;

int function_params_table_capacity;

void init_global_variable(void) {
    global_variable_count = 0;
    global_variable_capacity = 8;
    global_variable_table = malloc(sizeof(Variable) * 8);
}

static void make_bigger(void) {
    int new_capacity = global_variable_capacity * 2;
    Variable *new_global_variable_table = realloc(global_variable_table, sizeof(Variable) * new_capacity);

    if (new_global_variable_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize variable table\n");
        exit(1);
    }

    global_variable_capacity = new_capacity;
    global_variable_table = new_global_variable_table;
}

void init_local_variable(void) {
    local_variable_capacity = 8;
    local_scopes = malloc(sizeof(LocalVariables) * 8);
}

static void make_local_bigger(void) {
    int new_capacity = local_variable_capacity * 2;
    LocalVariables *new_local_variable_table = realloc(local_scopes, sizeof(LocalVariables) * new_capacity);

    if (new_local_variable_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize variable table\n");
        exit(1);
    }

    local_variable_capacity = new_capacity;
    local_scopes = new_local_variable_table;
}

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

int find_label(char *name) {
    for (int i = 0; i < label_count_internal; i++) {
        if (strncmp(symbol_table[i].name, name, strlen(symbol_table[i].name)) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1;
}

GlobalVariablesInfo find_global_variable(char *name) {
    GlobalVariablesInfo var;
    var.found = false;
    var.address = -1;
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

LocalVariablesInfo find_local_variable(char *name, int depth) {
    LocalVariablesInfo var;
    var.found = false;
    var.address = -1;
    for (int i = depth; i >= 0; i--) {
        for (int j = 0; j < local_scopes[i].variable_count; j++) {
            if (strcmp(local_scopes[i].name[j], name) == 0) {
                var.address = local_scopes[i].address[j];
                var.depth = i;
                var.found = true;
                var.type = local_scopes[i].type[j];
                return var;
            }
        }
    }
    return var;
}

int insert_global_variable(char *name, TypeKind* type) {
    int current_idx = global_variable_count;
    global_variable_count++;

    strcpy(global_variable_table[current_idx].name, name);
    global_variable_table[current_idx].type = *type;
    
    if (strncmp(name, "__s", 3) == 0) {
        global_variable_table[current_idx].memory_index = 1000 + (global_variable_count * 100);
    } else {
        global_variable_table[current_idx].memory_index = global_variable_count;
    }

    if (global_variable_count == global_variable_capacity) make_bigger();
    return global_variable_table[current_idx].memory_index;
}

int insert_local_variable(char *name, int depth, TypeKind* type) {
    if (depth == local_variable_capacity) make_local_bigger();

    int current_idx = local_scopes[depth].variable_count;
    local_scopes[depth].variable_count++;
    strcpy(local_scopes[depth].name[current_idx], name);
    local_scopes[depth].type[current_idx] = *type;
    
    if (strncmp(name, "__s", 3) == 0) {
        local_scopes[depth].address[current_idx] = 1000 + (current_idx * 100);;
    } else {
        local_scopes[depth].address[current_idx] = current_idx;
    }
    return local_scopes[depth].address[current_idx];
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
                return var;
            }
        }
    }

    return var;
}

void insert_function_params(char *name, Node *params, int depth) {
    if (depth == function_params_table_capacity) make_function_params_bigger();
    int current_idx = function_params_table[depth].function_count;
    

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
