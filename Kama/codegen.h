#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "opcode.h"
#include "string.h"

extern String *string_table;

extern int string_count;
extern int count;
extern int bytecode[1024];

void emit_no_operand(OpCode op_code);
void init_strings(void);
void generate_entry(Node *node);
void generate(Node *node);
#endif