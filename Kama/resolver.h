#ifndef RESOLVER_H
#define RESOLVER_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"
#include "token.h"

extern int block_depth;
void emit_count_reset();
void enter_scope();
void leave_scope();
void function_count_up();
void resolution_variable(Node* node, bool allow_create, TypeKind* type);
void name_resolution_binary(Node *node);
void name_resolution_func(Node *node, int address, int depth);
void param_name_resolution(Node *node, char* name, int address, int depth, TypeKind type);
void name_resolution(Node *node);


#endif