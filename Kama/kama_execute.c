#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//1.Opcodeの定義
typedef enum {
    OP_PUSH, //スタックに値を積む
    OP_ADD, //スタックの上の2つを足す
    OP_PRINT, //スタックのトップを表示
    OP_HALT //終了
} OpCode;

void run(int* program) {
    int stack[256]; //データの置き場
    int sp = -1; //スタックポインタ
    int pc = 0; //プログラムカウンタ (今何行目か)

    while (true) {
        int instruction = program[pc++]; //命令を取り出す

        switch (instruction) {
            case OP_PUSH:
                stack[++sp] = program[pc++]; //次の値をスタックへ
                break;
            case OP_ADD: {
                int b = stack[sp--]; //2つ取り出して
                int a = stack[sp--];
                stack[++sp] = a + b; //足して戻す
                break;
            }
            case OP_PRINT:
                printf("VM Output: %d\n", stack[sp]);
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