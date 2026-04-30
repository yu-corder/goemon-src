#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
    char name[32];
    int address;
} Label;

typedef struct {
    char name[32];
    int memory_index;
} Variable;

Variable variable_table[128];
int variable_count = 0;

Label symbol_table[128];
int label_count_internal = 0;

int find_label(char *name) {
    for (int i = 0; i < label_count_internal; i++) {
        if (strncmp(symbol_table[i].name, name, strlen(symbol_table[i].name)) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1;
}

int find_variable(char *name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variable_table[i].name, name) == 0) {
            return variable_table[i].memory_index;
        }
    }
    return -1;

}


int main() {
    FILE *src = fopen("examples/study.goe", "r");
    if (!src) { perror("Goemon's scroll (study.goe) not found"); return 1; }

    int bytecode[1024];
    int current_pc = 0;
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), src)) {
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';
            char *newline = strpbrk(line, "\r\n");
            if (newline) * newline = '\0';
            strcpy(symbol_table[label_count_internal].name, line);
            symbol_table[label_count_internal].address = current_pc;
            label_count_internal++;
        } else {
            if (strncmp(line, "push", 4) == 0) current_pc += 2;
            else if (strstr(line, "jmp") || strstr(line, "jz")) current_pc += 2;
            else if (strstr(line, "store") || strstr(line, "load")) {
                char var_name[32];
                int offset = (strncmp(line, "store", 5) == 0) ? 6 : 5;
                sscanf(line + offset, "%s", var_name);

                if (find_variable(var_name) == -1 ) {
                    strcpy(variable_table[variable_count].name, var_name);
                    variable_table[variable_count].memory_index = variable_count;
                    variable_count++;
                }
                current_pc += 2;
            } 
            else if (strlen(line) > 1) current_pc += 1;
        }
    }

    rewind(src);

    while (fgets(line, sizeof(line), src)) {
        if (strchr(line, ':')) continue;

        if (strncmp(line, "push '", 6) == 0) {
            char character = line[6];
            bytecode[count++] = OP_PUSH;
            bytecode[count++] = (int)character;
        } else if (strncmp(line, "push ", 5) == 0) {
            int val = atoi(line + 5);
            bytecode[count++] = OP_PUSH;
            bytecode[count++] = val;
        } else if (strstr(line, "add")) {
            bytecode[count++] = OP_ADD;
        } else if (strstr(line, "sub")) {
            bytecode[count++] = OP_SUB;
        } else if (strstr(line, "mul")) {
            bytecode[count++] = OP_MUL;
        } else if (strstr(line, "dup")) {
            bytecode[count++] = OP_DUP;
        } else if (strstr(line, "swap")) {
            bytecode[count++] = OP_SWAP;
        } else if (strstr(line, "pop")) {
            bytecode[count++] = OP_POP;
        } else if (strstr(line, "mod")) {
            bytecode[count++] = OP_MOD;
        } else if (strstr(line, "eq"))  {
            bytecode[count++] = OP_EQ;
        } else if (strstr(line, "ge"))  {
            bytecode[count++] = OP_GE;
        } else if (strstr(line, "le"))  {
            bytecode[count++] = OP_LE;
        } else if (strstr(line, "ne"))  {
            bytecode[count++] = OP_NE;
        } else if (strstr(line, "jmp")) {
            bytecode[count++] = OP_JMP;
            bytecode[count++] = find_label(line + 4);
        } else if (strstr(line, "jz")) {
            bytecode[count++] = OP_JZ;
            bytecode[count++] = find_label(line + 3);
        } else if (strncmp(line, "store", 5) == 0) {
            char var_name[32];
            sscanf(line + 6, "%s", var_name);
            bytecode[count++] = OP_STORE;
            bytecode[count++] = find_variable(var_name);
        } else if (strncmp(line, "load", 4) == 0) {
            char var_name[32];
            sscanf(line + 5, "%s", var_name);
            bytecode[count++] = OP_LOAD;
            bytecode[count++] = find_variable(var_name);
        } else if (strncmp(line, "lt", 2) == 0) {
            bytecode[count++] = OP_LT;
        } else if (strncmp(line, "gt", 2) == 0) {
            bytecode[count++] = OP_GT;
        } else if (strncmp(line, "input", 5) == 0) {
            bytecode[count++] = OP_INPUT;
        } else if (strncmp(line, "prints", 6) == 0) {
            bytecode[count++] = OP_PRINTS;
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