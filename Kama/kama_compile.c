#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { 
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_PRINT,
    OP_HALT 
} OpCode;

int main() {
    FILE *src = fopen("examples/study.goe", "r");
    if (!src) { perror("Goemon's scroll (study.goe) not found"); return 1; }

    int bytecode[1024];
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), src)) {
        if (strncmp(line, "push", 4) == 0) {
            int val = atoi(line + 5);
            bytecode[count++] = OP_PUSH;
            bytecode[count++] = val;
        } else if (strstr(line, "add")) {
            bytecode[count++] = OP_ADD;
        } else if (strstr(line, "sub")) {
            bytecode[count++] = OP_SUB;
        } else if (strstr(line, "print")) {
            bytecode[count++] = OP_PRINT;
        }
    }
    bytecode[count++] = OP_HALT;
    fclose(src);

    FILE *dest = fopen("examples/study.gb", "wb");
    fwrite(bytecode, sizeof(int), count, dest);
    fclose(dest);

    printf("絶景かな！ Compiled study.goe to study.gb\n");
    return 0;
}