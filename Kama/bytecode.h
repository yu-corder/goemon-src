#ifndef BYTECODE_H
#define BYTECODE_H

#include "opcode.h"
#include "type.h"

extern int *bytecode;

void init_bytecode(void);
void emit_no_operand(OpCode op_code);
void emit_one_operand (OpCode op_code, int *val);
void emit_two_operand(OpCode op_code, int *val1, int *val2);
void emit_two_operand_type(OpCode op_code, int *val1, TypeKind type);

#endif