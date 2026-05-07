#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

typedef enum {
    TK_PUSH,
    TK_INPUT,
    TK_STORE,
    TK_LOAD,
    TK_GE,
    TK_JZ,
    TK_HALT,
    TK_PRINTS,
    TK_NUMBER,
    TK_STRING,
    TK_IDENT,
    TK_COLON,
    TK_EOF,
} TokenKind;

typedef struct {
    TokenKind kind;
    int val;
    char str[256];
} Token;


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

    strcpy(variable_table[variable_count].name, name);
    
    if (strncmp(name, "__s", 3) == 0) {
        variable_table[variable_count].memory_index = 1000 + (variable_count * 100);
    } else {
        variable_table[variable_count].memory_index = variable_count;
    }

    return variable_table[variable_count++].memory_index;

}

Token tokens[256];
int tokenize (char *p) {
    int i = 0;
    while(*p) {
        if (isspace(*p)) { p++; continue;}

        if (isdigit(*p)) {
            tokens[i].kind = TK_NUMBER;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        if (strncmp(p, "push", 4) == 0 && isspace(p[4])) {
            printf("in if push\n");
            tokens[i++].kind = TK_PUSH;
            p += 4;
            continue;
        } 

        if (*p == '"') {
            printf("in if dubule\n");
            p++;
            int len = 0;
            while (*p != '"' && *p != '\0') {
                printf("in while\n");
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i].kind = TK_STRING;
            p++;
            i++;
            continue;
        }

        if (isalpha(*p) || *p == '_') {
            int len = 0;
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';

            if (*p == ':') {
                tokens[i].kind = TK_IDENT;
                i++;
                tokens[i++].kind = TK_COLON;
                p++;
            } else {
                tokens[i++].kind = TK_IDENT;
            }
            continue;
        }

        printf("不明な文字ですぞ: %c\n", *p);
    }
    tokens[i].kind = TK_EOF;
    return i;
}


int pos = 0;
Token* next_token() {
    return &tokens[pos++];
}

void collect_labels () {
    int count = 0;
    pos = 0;
    while (tokens[pos].kind != TK_EOF) {
        // int current_pos = pos;
        Token *t = next_token();

        if (t->kind == TK_PUSH) {
            count += 2;
            next_token();
        } else if (t->kind == TK_IDENT) {
            if (tokens[pos].kind == TK_COLON) {
                strcpy(symbol_table[label_count_internal].name, t->str);
                symbol_table[label_count_internal].address = count;
                label_count_internal++;
                next_token();
            } else {
                count += 2;
            }
        } else if (t->kind == TK_JZ) {
            count += 2;
            next_token();
        }
    }
}

void parse_generate () {
    int bytecode[1024];
    int count = 0;
    pos = 0;
    while (tokens[pos].kind != TK_EOF) {
        Token *t = next_token();

        switch(t->kind) {
            case TK_PUSH: {
                Token *num = next_token();
                if (num->kind != TK_NUMBER) {
                    printf("エラー: pushの次は数値を置いてくだされ");
                    exit(1);
                }
                bytecode[count++] = OP_PUSH;
                bytecode[count++] = num->val;
                break;
            }
            case TK_PRINTS: {
                printf("TK_PRINTS");
                break;
            }
            case TK_INPUT: {
                printf("TK_INPUT");
                break;
            }
            case TK_LOAD: {
                printf("TK_LOAD");
                break;
            }
            case TK_JZ: {
                printf("TK_JZ");
                break;
            }
            case TK_COLON: {
                printf("TK_COLON");
                break;
            }
            case TK_GE: {
                printf("TK_GE");
                break;
            }
            case TK_STORE: {
                printf("TK_STORE");
                break;
            }
            case TK_HALT: {
                printf("TK_HALT");
                break;
            }
            case TK_NUMBER: {
                printf("TK_NUMBER");
                break;
            }
            case TK_STRING: {
                printf("TK_STRING");
                break;
            }
            case TK_EOF: {
                printf("TK_EOF");
                break;
            }
            case TK_IDENT: {
                if (tokens[pos].kind == TK_COLON) {
                    next_token();
                }
                break;
            }
        }
    }

    // printf("%d", bytecode);

}

char *read_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror(path); exit(1); }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);
    return buf;
}

void debug_token(int count) {
    for (int i = 0; i <= count; i++) {
        printf("----debug-----\n");

        if (tokens[i].kind == TK_PUSH) {printf("TK_PUSH\n"); continue;}

        if (tokens[i].kind == TK_STRING) {
            printf("TK_STRING\n");
            printf("%s\n", tokens[i].str);
        }

        if (tokens[i].kind == TK_NUMBER) {
            printf("TK_NUMBER\n");
            printf("%d\n", tokens[i].val);
        }

        if (tokens[i].kind == TK_EOF) {
            printf("TK_EOF\n");
        }

        printf("----debug------\n");
    }
}


int main() {
    char *src = read_file("examples/study.goe");
    tokenize(src);
    collect_labels();
    parse_generate();
    printf("絶景かな！ Compiled study.goe to study.gb\n");
    return 0;
}