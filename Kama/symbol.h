#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"

typedef struct {
    char name[64][64];
    int address[64];
    int function_count;
    TypeKind type[64];
} Funcion;

extern Funcion *function_table;

typedef struct {
    char name[64][64];

    char params[16][16][32];
    int *param_count;
    int function_count;

    TypeKind **type;
    int **len;
} FuncionParams;

extern FuncionParams *function_params_table;


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
    int *len;
} FuncionParamsInfo;

void init_function_table(void);
void init_function_params_table(void);
void free_all_function_params(void);

FuncionParamsInfo find_function_params(char *name, int depth);
void insert_function_params(char *name, Node *params, int depth);
FuncionInfo find_function(char *name, int depth);
void insert_function(char *name, int address, int depth, TypeKind type);

#endif