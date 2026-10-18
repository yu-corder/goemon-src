#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "token.h"
Arena *head_arena = NULL;
Arena *current_arena = NULL;

void init_node_arena(void) {
    current_arena = malloc(sizeof(Arena));

    current_arena->node_tree = malloc(sizeof(Node) * 512);
    current_arena->capacity = 512;
    current_arena->used = 0;
    current_arena->next = NULL;

    head_arena = current_arena;
}

static void expand_arena(void) {
    Arena *tmp_arena = malloc(sizeof(Arena));
    tmp_arena->node_tree = malloc(sizeof(Node) * 512);
    tmp_arena->capacity = 512;
    tmp_arena->used = 0;
    tmp_arena->next = NULL;

    current_arena->next = tmp_arena;
    current_arena = tmp_arena;
}

static Node* arena_alloc_node() {
    if (current_arena->used == current_arena->capacity) {
        expand_arena();
    }

    Node *node = &current_arena->node_tree[current_arena->used++];
    memset(node, 0, sizeof(Node));
    return node;
}

Node* new_num_node (int *val) {
    Node *node = arena_alloc_node();

    node->kind = ND_NUM;
    node->val = *val;
    node->lhs = NULL;
    node->rhs = NULL;

    return node;
}

Node* new_bool_node (bool *val) {
    Node *node = arena_alloc_node();

    node->kind = ND_BOOL;
    node->bool_val = val;
    node->lhs = NULL;
    node->rhs = NULL;

    node->val = val ? 1 : 0;

    return node;
}

Node* new_str_node (char *str, int *len) {
    Node *node = arena_alloc_node();

    node->kind = ND_STR;
    strcpy(node->str, str);
    node->len = *len;
    node->lhs = NULL;
    node->rhs = NULL;

    return node;
}

Node* new_var_node (char *str, int *len) {
    Node *node = arena_alloc_node();

    node->kind = ND_VAR;
    node->lhs = NULL;
    node->rhs = NULL;
    node->len = *len;
    strcpy(node->name, str);

    return node;
}

Node* new_binary_node(NodeKind kind, Node* node1, Node* node2) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->lhs = node1;
    node->rhs = node2;

    return node;
}

Node* new_decl_no_assignment_node(NodeKind kind, Node* node1, TypeKind type) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->lhs = node1;
    node->type = type;

    return node;
}

Node* new_array_decl_node(NodeKind kind, Node* node1, int *len, TypeKind type) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->lhs = node1;
    node->len = *len;
    node->type = type;

    return node;
}

Node* new_array_node(NodeKind kind, Node* node1, char *str) {
    Node *node = arena_alloc_node();

    node->rhs = node1;
    node->kind = kind;
    strcpy(node->name, str);
    
    return node;
}

Node* new_decl_node(NodeKind kind, Node* node1, Node* node2, TypeKind type) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->lhs = node1;
    node->rhs = node2;
    node->type = type;

    return node;
}

Node* new_simple_node(NodeKind kind) {
    Node *node = arena_alloc_node();

    node->kind = kind;

    return node;
}

Node* new_unary_node(NodeKind kind, Node* node_lhs) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->lhs = node_lhs;

    return node;
}

Node* new_if_node(NodeKind kind, Node* condition, Node* then, Node* else_stmt) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->condition = condition;
    node->body = then;
    node->else_stmt = else_stmt;

    return node;
}

Node* new_loop_node(NodeKind kind, Node* condition, Node* body) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->condition = condition;
    node->body = body;

    return node;
}

Node* new_for_node(NodeKind kind, Node* init, Node* condition, Node* update, Node* body) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    node->init = init;
    node->condition = condition;
    node->update = update;
    node->body = body;

    return node;
}

Node* new_func_node(NodeKind kind, char *str, Node* params, Node* body, TypeKind type, int *len) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    strcpy(node->func_name, str);
    
    node->params = params;
    node->body = body;
    node->type = type;
    node->len = *len;

    return node;
}

Node* new_call_node(NodeKind kind, char *str, Node* params) {
    Node *node = arena_alloc_node();

    node->kind = kind;
    strcpy(node->func_name, str);

    node->params = params;

    return node;
}
