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
    OP_GE,
    OP_LE,
    OP_NE,
    OP_JMP,
    OP_JZ,
    OP_PRINT,
    OP_STORE,
    OP_LOAD,
    OP_LT,
    OP_GT,
    OP_INPUT,
    OP_PRINTS,
    OP_HALT
} OpCode;

void run(int* program) {
    int stack[1024];
    int memory[2048];
    int sp = -1;
    int pc = 0;

    for(int i = 0; i < 2048; i++) memory[i] = 0;
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
                if (sp < 1) {
                    printf("エラー: スタックが足りませぬ！ (pc=%d)\n", pc);
                    return;
                }
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
            case OP_GE: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = (a >= b) ? 1 : 0;
                break;
            }
            case OP_LE: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = (a <= b) ? 1 : 0;
                break;
            }
            case OP_NE: {
                int b = stack[sp--];
                int a = stack[sp--];
                stack[++sp] = (a != b) ? 1 : 0;
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
                int value = stack[sp--];
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
            case OP_INPUT: {
                char input_buf[100];
                printf("Input >>> ");
                if (fgets(input_buf, sizeof(input_buf), stdin)) {
                    int val;
                    if (sscanf(input_buf, "%d", &val) == 1) {
                        stack[++sp] = val;
                    }
                }
                break;
            }
            case OP_PRINTS: {
                int address = stack[sp--];
                while (memory[address] != 0) {
                    printf("%c", (char)memory[address]);
                    address++;
                }
                printf("\n");
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
