#ifndef LOCAL_VARIABLE_H
#define LOCAL_VARIABLE_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"


typedef struct {
    int variable_count;
    char **name;
    int *address;
    int variable_count_capacity;

    TypeKind *type;
} LocalVariables;

extern LocalVariables *local_scopes;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} LocalVariablesInfo;

extern int local_variable_capacity;


void init_local_variable(void);
void free_all_local_scopes(void);
LocalVariablesInfo find_local_variable(char *name, int depth);
int insert_local_variable(char *name, int depth, TypeKind* type, int len);
void make_local_bigger(void);

#endif