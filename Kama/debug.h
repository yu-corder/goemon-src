#ifndef DEBUG_H
#define DEBUG_H
#include "token.h"
#include "ast.h"
#include "type.h"

extern const char *token_kind_name[];

const char* token_name(TokenKind kind);
void debug_token(int count);
const char* type_name(TypeKind kind);
void print_indent(int depth);
void debug_ast_node(Node *node, int depth);
#endif