#ifndef GLOBAL_VARIABLE_H
#define GLOBAL_VARIABLE_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"


typedef struct {
    char *name;
    int memory_index;

    TypeKind type;
} Variable;

extern Variable *global_variable_table;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} GlobalVariablesInfo;


void init_global_variable(void);
void free_all_global_variable(void);
GlobalVariablesInfo find_global_variable(char *name);
int insert_global_variable(char *name, TypeKind* type, int len);

#endif