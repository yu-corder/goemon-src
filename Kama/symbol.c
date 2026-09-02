#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "symbol.h"

Label symbol_table[128];
Variable global_variable_table[128];
LocalVariables local_scopes[128];
Funcion function_table[128];
FuncionParams function_params_table[128];

int label_count_internal = 0;
int global_variable_count = 0;

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
    return global_variable_table[current_idx].memory_index;
}

int insert_local_variable(char *name, int depth, TypeKind* type) {
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
