#include <stdlib.h>
#include <stdio.h>

#include "bytecode.h"
#include "opcode.h"
#include "type.h"


int *bytecode;
int count = 0;
int bytecode_capacity;

void init_bytecode(void) {
    bytecode_capacity = 8;
    bytecode = malloc(sizeof(int) * 8);
}

static void make_bytecode_bigger(void) {
    int new_capacity = bytecode_capacity * 2;
    int *new_bytecode = realloc(bytecode, sizeof(int) * new_capacity);

    if (new_bytecode ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize function table\n");
        exit(1);
    }

    bytecode_capacity = new_capacity;
    bytecode = new_bytecode;
}

static void emit(int value) {
    if (count == bytecode_capacity) {
        make_bytecode_bigger();
    }

    bytecode[count++] = value;
}

void emit_no_operand(OpCode op_code) {
    emit(op_code);
}

void emit_one_operand (OpCode op_code, int *val) {
    emit(op_code);

    if (val != NULL) {
        emit(*val);
    }
}

void emit_two_operand(OpCode op_code, int *val1, int *val2) {
    emit(op_code);
    emit(*val1);
    emit(*val2);
}

void emit_two_operand_type(OpCode op_code, int *val1, TypeKind type) {
    emit(op_code);
    emit(*val1);
    emit(type);
}