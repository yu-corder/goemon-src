#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>
#include "token.h"
#include "ast.h"
#include "type.h"

extern bool g_debug_ast;
extern bool g_debug_token;
extern bool g_debug_binary;

const char* token_name(TokenKind kind);
void debug_token(int count);
const char* type_name(TypeKind kind);
void debug_ast_node(Node *node, int depth);
void debug_binary();
#endif