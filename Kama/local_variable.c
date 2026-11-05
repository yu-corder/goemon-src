#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "local_variable.h"

LocalVariables *local_scopes;

int local_variable_capacity;

void init_local_variable(void) {
    local_variable_capacity = 8;
    local_scopes = malloc(sizeof(LocalVariables) * 8);

    if (local_scopes == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to allocate local variable scopes\n");
        exit(1);
    }

    for (int i = 0; i < local_variable_capacity; i++) {
        local_scopes[i].variable_count_capacity = 32;

        TypeKind *type = malloc(sizeof(TypeKind) * local_scopes[i].variable_count_capacity);

        if (type == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate local variable type\n");
            exit(1);
        }

        local_scopes[i].type = type;

        int *address = malloc(sizeof(int) * local_scopes[i].variable_count_capacity);

        if (address == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate local variable address\n");
            exit(1);
        }

        local_scopes[i].address = address;

        char **name = malloc(sizeof(char *) * local_scopes[i].variable_count_capacity);

        if (name == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate local variable name\n");
            exit(1);
        }
        local_scopes[i].name = name;

        local_scopes[i].variable_count = 0;
    }
}

void free_all_local_scopes(void) {
    for (int i = 0; i < local_variable_capacity; i++) {
        for (int j = 0; j < local_scopes[i].variable_count; j++) {
            free(local_scopes[i].name[j]);
        }
        free(local_scopes[i].type);
        free(local_scopes[i].address);
        free(local_scopes[i].name);
        local_scopes[i].address = NULL;
        local_scopes[i].type = NULL;
        local_scopes[i].name = NULL;
    }

    free(local_scopes);
}

void make_local_bigger(void) {
    int old_capacity = local_variable_capacity;
    int new_capacity = local_variable_capacity * 2;
    LocalVariables *new_local_variable_table = realloc(local_scopes, sizeof(LocalVariables) * new_capacity);

    if (new_local_variable_table ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize variable table\n");
        exit(1);
    }

    local_scopes = new_local_variable_table;

    for (int i = old_capacity; i < new_capacity; i++) {
        local_scopes[i].variable_count_capacity = 32;

        TypeKind *type = malloc(sizeof(TypeKind) * local_scopes[i].variable_count_capacity);

        if (type == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate local variable type\n");
            exit(1);
        }

        local_scopes[i].type = type;

        int *address = malloc(sizeof(int) * local_scopes[i].variable_count_capacity);

        if (address == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate local variable address\n");
            exit(1);
        }

        local_scopes[i].address = address;

        char **name = malloc(sizeof(char *) * local_scopes[i].variable_count_capacity);

        if (name == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate local variable name\n");
            exit(1);
        }
        local_scopes[i].name = name;
        local_scopes[i].variable_count = 0;
    }

    local_variable_capacity = new_capacity;
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

int insert_local_variable(char *name, int depth, TypeKind* type, int len) {
    if (len < 0) {
        fprintf(stderr, "Runtime Error: Invalid variable name length\n");
        exit(1);
    }
    
    while (depth >= local_variable_capacity) {
        make_local_bigger();
    }

    int current_idx = local_scopes[depth].variable_count;
    int current_variable_count_capacity = local_scopes[depth].variable_count_capacity;

    if (current_idx >= current_variable_count_capacity) {

        int new_variable_count_capacity = current_variable_count_capacity * 2;

        TypeKind *new_type = realloc(
            local_scopes[depth].type,
            sizeof(TypeKind) * new_variable_count_capacity
        );

        if (new_type == NULL) {
            fprintf(stderr, "Runtime Error: Failed to resize Local Variables Type\n");
            exit(1); 
        }
        local_scopes[depth].type = new_type;

        int *new_address = realloc(
            local_scopes[depth].address,
            sizeof(int) * new_variable_count_capacity
        );

        if (new_address == NULL) {
            fprintf(stderr, "Runtime Error: Failed to resize Local Variables Address\n");
            exit(1);
        }
        local_scopes[depth].address = new_address;

        char **new_name = realloc(
            local_scopes[depth].name,
            sizeof(char *) * new_variable_count_capacity
        );

        if (new_name == NULL) {
            fprintf(stderr, "Runtime Error: Failed to resize Local Variables Name\n");
            exit(1);
        }
        local_scopes[depth].name = new_name;

        local_scopes[depth].variable_count_capacity = new_variable_count_capacity;
    }

    char *new_name = malloc(len + 1);
    if (new_name == NULL) {
        fprintf(stderr, "Runtime Error: Failed to allocate variable name\n");
        exit(1);
    }

    local_scopes[depth].name[current_idx] = new_name;
    strcpy(local_scopes[depth].name[current_idx], name);
    
    local_scopes[depth].type[current_idx] = *type;
    local_scopes[depth].address[current_idx] = current_idx;

    local_scopes[depth].variable_count++;

    return local_scopes[depth].address[current_idx];
}