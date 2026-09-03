#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "token.h"
static int node_depth = 0;
Node node_tree[128];

Node* new_num_node (int *val) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_NUM;
    node_tree[current_idx].val = *val;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;

    return &node_tree[current_idx];
}

Node* new_bool_node (bool *val) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_BOOL;
    node_tree[current_idx].bool_val = val;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;

    node_tree[current_idx].val = val ? 1 : 0;

    return &node_tree[current_idx];
}

Node* new_str_node (char *str, int *len) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_STR;
    strcpy(node_tree[current_idx].str, str);
    node_tree[current_idx].len = *len;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;

    return &node_tree[current_idx];
}

Node* new_var_node (char *str) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_VAR;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;
    strcpy(node_tree[current_idx].name, str);

    return &node_tree[current_idx];
}

Node* new_binary_node(NodeKind kind, Node* node1, Node* node2) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node1;
    node_tree[current_idx].rhs = node2;

    return &node_tree[current_idx];
}

Node* new_decl_no_assignment_node(NodeKind kind, Node* node1, TypeKind type) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node1;
    node_tree[current_idx].type = type;

    return &node_tree[current_idx];
}

Node* new_decl_node(NodeKind kind, Node* node1, Node* node2, TypeKind type) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node1;
    node_tree[current_idx].rhs = node2;
    node_tree[current_idx].type = type;

    return &node_tree[current_idx];
}

Node* new_simple_node(NodeKind kind) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;

    return &node_tree[current_idx];
}

Node* new_unary_node(NodeKind kind, Node* node) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node;

    return &node_tree[current_idx];
}

Node* new_if_node(NodeKind kind, Node* condition, Node* then, Node* else_stmt) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].condition = condition;
    node_tree[current_idx].body = then;
    node_tree[current_idx].else_stmt = else_stmt;

    return &node_tree[current_idx];
}

Node* new_loop_node(NodeKind kind, Node* condition, Node* body) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].condition = condition;
    node_tree[current_idx].body = body;

    return &node_tree[current_idx];
}

Node* new_for_node(NodeKind kind, Node* init, Node* condition, Node* update, Node* body) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].init = init;
    node_tree[current_idx].condition = condition;
    node_tree[current_idx].update = update;
    node_tree[current_idx].body = body;

    return &node_tree[current_idx];
}

Node* new_func_node(NodeKind kind, char *str, Node* params, Node* body, TypeKind type) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    strcpy(node_tree[current_idx].func_name, str);
    
    node_tree[current_idx].params = params;
    node_tree[current_idx].body = body;
    node_tree[current_idx].type = type;

    return &node_tree[current_idx];
}

Node* new_call_node(NodeKind kind, char *str, Node* params) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    strcpy(node_tree[current_idx].func_name, str);

    node_tree[current_idx].params = params;

    return &node_tree[current_idx];
}
