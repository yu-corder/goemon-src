#ifndef SYMBOL_H
#define SYMBOL_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"

typedef struct {
    int param_count;
    char **name;
    int *len;
    TypeKind *type;
} Params;

typedef struct {
    char **name;
    int *address;
    int function_count;
    TypeKind *type;

    Params *params;
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
    
    //後で使う(関数定義と呼び出し側の引数の個数チェック)
    int param_count;

    TypeKind type;
    bool found;
} FuncionInfo;

typedef struct {
    int address;
    int depth;
    int param_count;

    char **params;

    bool found;

    TypeKind *type;
    int *len;
} FuncionParamsInfo;

void init_function_table(void);
void free_all_function_table(void);

FuncionParamsInfo find_function_params(char *name, int depth);
FuncionInfo find_function(char *name, int depth);
void insert_function(char *name, Node *params, int address, int depth, TypeKind type, int len);

#endif