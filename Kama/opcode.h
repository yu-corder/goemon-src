#ifndef OPCODE_H
#define OPCODE_H

typedef enum { 
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_DUP,
    OP_SWAP,
    OP_POP,
    OP_MOD,
    OP_EQ,
    OP_GE,
    OP_LE,
    OP_NE,
    OP_JMP,
    OP_JZ,
    OP_PRINT,
    OP_STORE,
    OP_STORE_LOCAL,
    OP_LOAD,
    OP_LOAD_LOCAL,
    OP_LT,
    OP_GT,
    OP_INC,
    OP_INC_LOCAL,
    OP_INPUT,
    OP_PRINTS,
    OP_CALL,
    OP_RET,
    OP_PRINT_STRING,
    OP_HALT 
} OpCode;

#endif