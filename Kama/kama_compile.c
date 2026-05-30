#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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
    TK_JZ,
    TK_HALT,
    TK_PRINTS,
    TK_PRINT,
    TK_NUMBER,
    TK_STRING,
    TK_IDENT,
    TK_COLON,
    TK_ASSIGN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_MOD,
    TK_SEMI,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    TK_EQ,
    TK_NE,
    TK_IF,
    TK_ELSE,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
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
            tokens[i++].kind = TK_PUSH;
            p += 4;
            continue;
        }

        if (strncmp(p, "halt", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            tokens[i++].kind = TK_HALT;
            p += 4;
            continue;
        }

        if (strncmp(p, "print", 5) == 0 && (isspace(p[5]) || p[5] == '\0')) {
            tokens[i++].kind = TK_PRINT;
            p += 5;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && (isspace(p[2]) || p[2] == '\0')) {
            tokens[i++].kind = TK_IF;
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            tokens[i++].kind = TK_ELSE;
            p += 4;
            continue;
        }

        if (*p == '(') {
            tokens[i++].kind = TK_LPAREN;
            p++;
            continue;
        }

        if (*p == ')') {
            tokens[i++].kind = TK_RPAREN;
            p++;
            continue;
        }

        if (*p == '{') {
            tokens[i++].kind = TK_LBRACE;
            p++;
            continue;
        }

        if (*p == '}') {
            tokens[i++].kind = TK_RBRACE;
            p++;
            continue;
        }

        if (*p == '+') {
            tokens[i++].kind = TK_PLUS;
            p++;
            continue;
        }

        if (*p == '-') {
            tokens[i++].kind = TK_MINUS;
            p++;
            continue;
        }

        if (*p == '*') {
            tokens[i++].kind = TK_MUL;
            p++;
            continue;
        }

        if (*p == '/') {
            tokens[i++].kind = TK_DIV;
            p++;
            continue;
        }

        if (*p == '%') {
            tokens[i++].kind = TK_MOD;
            p++;
            continue;
        }

        if (*p == ';') {
            tokens[i++].kind = TK_SEMI;
            p++;
            continue;
        }

        if (*p == '<') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_LE;
                p++;
            } else {
                tokens[i++].kind = TK_LT;
            }
            continue;
        }

        if (*p == '>') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_GE;
                p++;
            } else {
                tokens[i++].kind = TK_GT;
            }
            continue;
        }

        if (*p == '"') {
            p++;
            int len = 0;
            while (*p != '"' && *p != '\0') {
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

        if (*p == '!') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_NE;
                p++;
            }
            continue;
        }

        if (*p == '=') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_EQ;
                p++;
            } else {
                tokens[i++].kind = TK_ASSIGN;
            }
            continue;
        }
        p++;
        printf("不明な文字ですぞ: %c\n", *p);
    }
    tokens[i].kind = TK_EOF;
    return i;
}


int pos = 0;
Token* next_token() {
    return &tokens[pos++];
}

bool is_first_pass = true;

int bytecode[1024];
int count = 0;
void emit_op (OpCode op_code, int *val) {

    if (is_first_pass) {
        count++;
        if (val != NULL) {
            count++;
        }
    } else {
        bytecode[count++] = op_code;
        if (val != NULL) {
            bytecode[count++] = *val;
        }
    }
}

void parse_primary() {
    if (tokens[pos].kind == TK_PLUS) {
        next_token();
    }

    Token *t = next_token();
    if (t->kind == TK_NUMBER) {
        emit_op(OP_PUSH, &t->val);
    } else if (t->kind == TK_IDENT) {
        int addr = find_variable(t->str);
        emit_op(OP_LOAD, &addr);
    }
}

void parse_term() {
    parse_primary();

    while(tokens[pos].kind == TK_MUL || tokens[pos].kind == TK_DIV || tokens[pos].kind == TK_MOD) {
        TokenKind kind_type = tokens[pos].kind;
        next_token();
        parse_primary();

        if (kind_type == TK_MUL) {
            emit_op(OP_MUL, NULL);
        } else if (kind_type == TK_DIV) {
            emit_op(OP_DIV, NULL);
        } else if (kind_type == TK_MOD) {
            emit_op(OP_MOD, NULL);
        }
    }
}


void parse_expression() {
    parse_term();

    while (tokens[pos].kind == TK_PLUS || tokens[pos].kind == TK_MINUS) {
        TokenKind kind_type = tokens[pos].kind;
        next_token();
        parse_term();

        if (kind_type == TK_PLUS) {
            emit_op(OP_ADD, NULL);
        } else if (kind_type == TK_MINUS) {
            emit_op(OP_SUB, NULL);
        }
    }
}

void parse_evaluation() {
    parse_expression();

    if (tokens[pos].kind == TK_LT) {
        next_token();
        parse_expression();
        emit_op(OP_LT, NULL);
    } else if (tokens[pos].kind == TK_GT) {
        next_token();
        parse_expression();
        emit_op(OP_GT, NULL);
    } else if (tokens[pos].kind == TK_LE) {
        next_token();
        parse_expression();
        emit_op(OP_LE, NULL);
    } else if (tokens[pos].kind == TK_GE) {
        next_token();
        parse_expression();
        emit_op(OP_GE, NULL);
    } else if (tokens[pos].kind == TK_EQ) {
        next_token();
        parse_expression();
        emit_op(OP_EQ, NULL);
    } else if (tokens[pos].kind == TK_NE) {
        next_token();
        parse_expression();
        emit_op(OP_NE, NULL);
    }
}

void parse_if();

void parse_statement() {
    Token *t = next_token();
    switch(t->kind) {
        case TK_PUSH: {
            Token *num = next_token();
            if (num->kind != TK_NUMBER) {
                printf("エラー: pushの次は数値を置いてくだされ");
                exit(1);
            }
            emit_op(OP_PUSH, &num->val);
            break;
        }
        case TK_PRINT: {
            Token *value = next_token();
            if (value->kind == TK_IDENT) {
                int addr = find_variable(value->str);
                emit_op(OP_LOAD, &addr);
            } else if (value->kind == TK_NUMBER) {
                emit_op(OP_PUSH, &value->val);
            }
            emit_op(OP_PRINT, NULL);
            break;
        }
        case TK_IDENT: {
            if (tokens[pos].kind == TK_COLON) {
                next_token();
            }

            if (tokens[pos].kind == TK_ASSIGN) {
                next_token();
                parse_evaluation();
                if (tokens[pos].kind == TK_SEMI) {
                    //ここに入った時点でOP_STOREは確定

                    int addr = find_variable(t->str);
                    emit_op(OP_STORE, &addr);
                }
            }
            break;
        }
        case TK_IF: {
            parse_if();
            break;
        }
        default:
            break;
    }
}

void parse_if () {
    if (tokens[pos].kind == TK_LPAREN) next_token();
    parse_evaluation();
    if (tokens[pos].kind == TK_RPAREN) next_token();

    int my_jz_idx = count;
    int zero = 0;
    emit_op(OP_JZ, &zero);

    if (tokens[pos].kind == TK_LBRACE) next_token();
    while (tokens[pos].kind != TK_RBRACE && tokens[pos].kind != TK_EOF) {
        parse_statement();
    }
    if (tokens[pos].kind == TK_RBRACE) next_token();

    if (tokens[pos].kind == TK_ELSE) {
        next_token();

        int my_jmp_idx = count;
        emit_op(OP_JMP, &zero);
        if (!is_first_pass) {
            bytecode[my_jz_idx + 1] = count; 
        }

        if (tokens[pos].kind == TK_IF) {
            next_token();
            parse_if();
            if (!is_first_pass) {
                bytecode[my_jmp_idx + 1] = count;
            }
        } else {
            if (tokens[pos].kind == TK_LBRACE) next_token();
            while (tokens[pos].kind != TK_RBRACE && tokens[pos].kind != TK_EOF) {
                parse_statement();
            }
            if (tokens[pos].kind == TK_RBRACE) next_token();

            if (!is_first_pass) {
                bytecode[my_jmp_idx + 1] = count;
            }
        }

    } else {
        if (!is_first_pass) {
            bytecode[my_jz_idx + 1] = count;
        }
    }
}

void parse_program () {
    pos = 0;
    while (tokens[pos].kind != TK_EOF) {
        parse_statement();
    }
    emit_op(OP_HALT, NULL);

    if (!is_first_pass) {
        for (int i = 0; i < count; i++) {
            printf("%d\n", bytecode[i]);
        }
    }

    FILE *dest = fopen("examples/study.gb", "wb");
    fwrite(bytecode, sizeof(int), count, dest);
    fclose(dest);
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
        printf("----debug start-----\n");

        if (tokens[i].kind == TK_PUSH) {printf("TK_PUSH\n");}

        if (tokens[i].kind == TK_PLUS) {
            printf("TK_PLUS\n");
        }

        if (tokens[i].kind == TK_MINUS) {
            printf("TK_MINUS\n");
        }

        if (tokens[i].kind == TK_MUL) {
            printf("TK_MUL\n");
        }

        if (tokens[i].kind == TK_DIV) {
            printf("TK_DIV\n");
        }

        if (tokens[i].kind == TK_MOD) {
            printf("TK_MOD\n");
        }

        if (tokens[i].kind == TK_LT) {
            printf("TK_LT\n");
        }

        if (tokens[i].kind == TK_LE) {
            printf("TK_LE\n");
        }

        if (tokens[i].kind == TK_GT) {
            printf("TK_GT\n");
        }

        if (tokens[i].kind == TK_EQ) {
            printf("TK_EQ\n");
        }

        if (tokens[i].kind == TK_NE) {
            printf("TK_NE\n");
        }

        if (tokens[i].kind == TK_SEMI) {
            printf("TK_SEMI\n");
        }

        if (tokens[i].kind == TK_STRING) {
            printf("TK_STRING\n");
            printf("%s\n", tokens[i].str);
        }

        if (tokens[i].kind == TK_ASSIGN) {
            printf("TK_ASSIGN\n");
        }

        if (tokens[i].kind == TK_IDENT) {
            printf("TK_IDENT\n");
            printf("%s\n", tokens[i].str);
        }

        if (tokens[i].kind == TK_PRINT) {
            printf("TK_PRINT\n");
        }

        if (tokens[i].kind == TK_NUMBER) {
            printf("TK_NUMBER\n");
            printf("%d\n", tokens[i].val);
        }

        if (tokens[i].kind == TK_EOF) {
            printf("TK_EOF\n");
        }

        if (tokens[i].kind == TK_IF) {
            printf("TK_IF\n");
        }

        if (tokens[i].kind == TK_ELSE) {
            printf("TK_ELSE\n");
        }

        if (tokens[i].kind == TK_LPAREN) {
            printf("TK_LPAREN\n");
        }

        if (tokens[i].kind == TK_RPAREN) {
            printf("TK_RPAREN\n");
        }

        if (tokens[i].kind == TK_LBRACE) {
            printf("TK_LBRACE\n");
        }

        if (tokens[i].kind == TK_RBRACE) {
            printf("TK_RPAREN\n");
        }

        if (tokens[i].kind == TK_HALT) {
            printf("TK_HALT\n");
        }

        printf("----debug end------\n");
    }
}


int main() {
    char *src = read_file("examples/study.goe");
    int cn = tokenize(src);
    debug_token(cn);

    is_first_pass = true;
    count = 0;
    pos = 0;
    parse_program();

    is_first_pass = false;
    count = 0;
    pos = 0;
    parse_program();

    printf("絶景かな！ Compiled study.goe to study.gb\n");
    return 0;
}