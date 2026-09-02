#ifndef AST_H
#define AST_H

#include <stdbool.h>
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
extern int node_depth;

Node* new_num_node (int *val);
Node* new_bool_node ();
Node* new_str_node (char *str, int *len);
Node* new_var_node (char *str);
Node* new_binary_node(NodeKind kind, Node* node1, Node* node2);
Node* new_decl_no_assignment_node(NodeKind kind, Node* node1, TypeKind type);
Node* new_decl_node(NodeKind kind, Node* node1, Node* node2, TypeKind type);
Node* new_simple_node(NodeKind kind);
Node* new_unary_node(NodeKind kind, Node* node);
Node* new_if_node(NodeKind kind, Node* condition, Node* then, Node* else_stmt);
Node* new_loop_node(NodeKind kind, Node* condition, Node* body);
Node* new_for_node(NodeKind kind, Node* init, Node* condition, Node* update, Node* body);
Node* new_func_node(NodeKind kind, char *str, Node* params, Node* body, TypeKind type);
Node* new_call_node(NodeKind kind, char *str, Node* params);

#endif