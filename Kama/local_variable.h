#ifndef LOCAL_VARIABLE_H
#define LOCAL_VARIABLE_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"


typedef struct {
    int variable_count;
    char name[32][32];
    int address[32];

    TypeKind type[32];
} LocalVariables;

extern LocalVariables *local_scopes;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} LocalVariablesInfo;


void init_local_variable(void);
LocalVariablesInfo find_local_variable(char *name, int depth);
int insert_local_variable(char *name, int depth, TypeKind* type);

#endif