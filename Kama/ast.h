#ifndef AST_H
#define AST_H

#include "type.h"

typedef enum {
    ND_NUM,
    ND_STR,
    ND_BOOL,
    ND_ADD,
    ND_MINUS,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_LT,
    ND_GT,
    ND_LE,
    ND_GE,
    ND_EQ,
    ND_NE,
    ND_VAR_DECL,
    ND_ASSIGN,
    ND_VAR,
    ND_PRINT,
    ND_IF,
    ND_BLOCK,
    ND_INC,
    ND_WHILE,
    ND_BREAK,
    ND_CONTINUE,
    ND_FOR,
    ND_FUNCTION,
    ND_CALL,
    ND_RET,
} NodeKind;

typedef struct Node {
    NodeKind kind;

    struct Node *lhs;
    struct Node *rhs;

    struct Node *condition;
    struct Node *body;
    struct Node *else_stmt;

    struct Node *init;
    struct Node *update;

    struct Node *params;
    

    struct Node *next;

    int val;
    bool bool_val;
    char str[128];
    int len;
    char name[32];
    char func_name[64];

    TypeKind type;

    int address;
    int depth;
    bool is_global;
} Node;

#endif