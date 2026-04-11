#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DUP,
    OP_SWAP,
    OP_POP,
    OP_MOD,
    OP_EQ,
    OP_JMP,
    OP_JZ,
    OP_PRINT,
    OP_STORE,
    OP_LOAD,
    OP_LT,
    OP_GT,
    OP_HALT
} OpCode;

void run(int* program) {
    int stack[256];
    int memory[256];
    int sp = -1;
    int pc = 0;

    while (true) {
        int instruction = program[pc++];

        switch (instruction) {
            case OP_PUSH:
                stack[++sp] = program[pc++];
                if (sp >= 255) {
                    printf("限界突破！スタックが溢れましたぞ！ (pc=%d)\n", pc);
                    return;
                }
                break;
            case OP_ADD: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = a + b;
                break;
            }
            case OP_SUB: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = a - b;
                break;
            }
            case OP_MUL: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = a * b;
                break;
            }
            case OP_DUP: {
                int top = stack[sp];
                stack[++sp] = top;
                break;
            }
            case OP_SWAP: {
                int temp = stack[sp];
                stack[sp] = stack[sp - 1];
                stack[sp - 1] = temp;
                break;
            }
            case OP_MOD: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = a % b;
                break;
            }
            case OP_POP: {
                if (sp >= 0) {
                    sp--;
                }
                break;
            }
            case OP_EQ: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = (a == b) ? 1 : 0;
                break;
            }
            case OP_JMP: {
                int target_pc = program[pc++];
                pc = target_pc;
                break;
            }
            case OP_JZ: {
                int target_pc = program[pc++];
                int condtion = stack[sp--];

                if (condtion == 0) {
                    pc = target_pc;
                }
                break;
            }
            case OP_STORE: {
                int address = program[pc++];
                int value = stack[sp];
                memory[address] = value;
                break;
            }
            case OP_LOAD: {
                int address = program[pc++];
                stack[++sp] = memory[address];
                break;
            }
            case OP_LT: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = (a < b) ? 1 : 0;
                break;
            }
            case OP_GT: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = (a > b) ? 1 : 0;
                break;
            }
            case OP_PRINT:
                if (sp < 0) {
                    printf("エラー：スタックが空なのに print しようとしましたぞ！\n");
                } else {
                    printf("VM Output: %d\n", stack[sp]);
                }
                break;
            case OP_HALT:
                return;
        }
    }
}

void load_and_run(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    int* code = malloc(size);
    fread(code, 1, size, f);
    fclose(f);

    run(code);

    free(code);
}

int main (int argc, char** argv) {
    if (argc < 2) { printf("Usage: %s <file.gb>\n", argv[0]); return 1;}
    load_and_run(argv[1]);
    return 0;
}