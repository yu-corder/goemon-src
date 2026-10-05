#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local_variable.h"

LocalVariables *local_scopes;

int local_variable_capacity;
int variable_count_capacity;
int max_depth = 0;

void init_local_variable(void) {
    local_variable_capacity = 8;
    local_scopes = malloc(sizeof(LocalVariables) * 8);

    variable_count_capacity = 32;
    for (int i = 0; i < local_variable_capacity; i++) {
        local_scopes[i].type = malloc(sizeof(TypeKind) * 32);
        local_scopes[i].address = malloc(sizeof(int) * 32);
    }
}

void free_all_local_scopes(void) {
    for (int i = 0; i < max_depth; i++) {
        free(local_scopes[i].type);
        free(local_scopes[i].address);
        local_scopes[i].address = NULL;
        local_scopes[i].type = NULL;
    }

    free(local_scopes);
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

int insert_local_variable(char *name, int depth, TypeKind* type) {
    if (depth == local_variable_capacity) make_local_bigger();
    if (max_depth <= depth) max_depth = depth;

    int current_idx = local_scopes[depth].variable_count;

    if (current_idx == variable_count_capacity) {
        int new_variable_count_capacity = variable_count_capacity * 2;
        local_scopes[depth].type = realloc(
            local_scopes[depth].type,
            sizeof(TypeKind) * new_variable_count_capacity
        );
        
        local_scopes[depth].address = realloc(
            local_scopes[depth].address,
            sizeof(int) * new_variable_count_capacity
        );
        variable_count_capacity = new_variable_count_capacity;
    }

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