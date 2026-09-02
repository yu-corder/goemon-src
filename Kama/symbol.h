#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"
#include "token.h"


typedef struct {
    char name[32];
    int address;
} Label;

extern Label symbol_table[128];

typedef struct {
    char name[32];
    int memory_index;

    TypeKind type;
} Variable;

extern Variable global_variable_table[128];

typedef struct {
    int variable_count;
    char name[32][32];
    int address[32];

    TypeKind type[32];
} LocalVariables;

extern LocalVariables local_scopes[128];

typedef struct {
    char name[64][64];
    int address[64];
    int function_count;
    TypeKind type[64];
} Funcion;

extern Funcion function_table[128];

typedef struct {
    char name[64][64];

    char params[16][16][32];
    int param_count[64];
    int function_count;

    TypeKind type[16][16];
} FuncionParams;

extern FuncionParams function_params_table[128];


typedef struct {
    int address;
    int depth;
    int param_count;

    char (*params)[32];

    TypeKind type;
    bool found;
} FuncionInfo;

typedef struct {
    int address;
    int depth;
    int param_count;

    char (*params)[32];

    bool found;

    TypeKind *type;
} FuncionParamsInfo;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} LocalVariablesInfo;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} GlobalVariablesInfo;

int find_label(char *name);

GlobalVariablesInfo find_global_variable(char *name);
LocalVariablesInfo find_local_variable(char *name, int depth);
int insert_global_variable(char *name, TypeKind* type);
int insert_local_variable(char *name, int depth, TypeKind* type);
FuncionParamsInfo find_function_params(char *name, int depth);
void insert_function_params(char *name, Node *params, int depth);
FuncionInfo find_function(char *name, int depth);
void insert_function(char *name, int address, int depth, TypeKind type);

#endif