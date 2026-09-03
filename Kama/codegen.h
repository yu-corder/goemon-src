#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdbool.h>
#include "token.h"
#include "ast.h"
#include "type.h"
#include "opcode.h"
#include "string.h"

extern String string_table[128];

extern int string_count;
extern int count;
extern int bytecode[1024];

void emit_no_operand(OpCode op_code);
void generate(Node *node);
#endif