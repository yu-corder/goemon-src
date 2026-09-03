#ifndef RESOLVER_H
#define RESOLVER_H

#include "ast.h"

extern int block_depth;
void emit_count_reset();
void enter_scope();
void leave_scope();
void function_count_up();
void name_resolution(Node *node);


#endif