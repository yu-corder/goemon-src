#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "debug.h"

Token *tokens;
int token_capacity;

int line = 1;
int i = 0;

static void init_tokens(void) {
    token_capacity = INITIAL_TOKEN_CAPACITY;
    tokens = malloc(sizeof(Token) * INITIAL_TOKEN_CAPACITY);
}

static void make_bigger(void) {
    int new_capacity = token_capacity * 2;
    Token *new_tokens = realloc(tokens, sizeof(Token) * new_capacity);

    if (new_tokens ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize tokens\n");
        exit(1);
    }

    token_capacity = new_capacity;
    tokens = new_tokens;
}

static void ensure_token_capacity(void) {
    i++;
    if (i == token_capacity) {
        make_bigger();
    } 
}

static char *scan_keyword(char *p, TokenKind kind, int length) {
    tokens[i].kind = kind;
    tokens[i].line = line;
    ensure_token_capacity();
    return p + length;
}

void tokenize (char *p) {
    init_tokens();
    while(*p) {
        if (*p == '\n') {
            p++;
            line++;
            continue;
        }

        if (isspace(*p)) { p++; continue;}

        if (isdigit(*p)) {
            tokens[i].kind = TK_NUMBER;
            tokens[i].val = strtol(p, &p, 10);
            tokens[i].line = line;
            ensure_token_capacity();
            continue;
        }

        if (strncmp(p, "halt", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            p = scan_keyword(p, TK_HALT, 4);
            continue;
        }

        if (strncmp(p, "print", 5) == 0 && (isspace(p[5]) || p[5] == '\0')) {
            p = scan_keyword(p, TK_PRINT, 5);
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && (isspace(p[2]) || p[2] == '\0')) {
            p = scan_keyword(p, TK_IF, 2);
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            p = scan_keyword(p, TK_ELSE, 4);
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && (isspace(p[5]) || p[5] == '\0')) {
            p = scan_keyword(p, TK_WHILE, 5);
            continue;
        }

        if (strncmp(p, "break", 5) == 0 && (isspace(p[5]) || p[5] == '\0' || p[5] == ';')) {
            p = scan_keyword(p, TK_BREAK, 5);
            continue;
        }

        if (strncmp(p, "continue", 8) == 0 && (isspace(p[8]) || p[8] == '\0' || p[8] == ';')) {
            p = scan_keyword(p, TK_CONTINUE, 8);
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            p = scan_keyword(p, TK_FOR, 3);
            continue;
        }

        if (strncmp(p, "function", 8) == 0 && (isspace(p[8]) || p[8] == '\0')) {
            p = scan_keyword(p, TK_FUNCTION, 8);
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && (isspace(p[6]) || p[6] == '\0')) {
            p = scan_keyword(p, TK_RET, 6);
            continue;
        }

        if (strncmp(p, "int", 3) == 0 && (isspace(p[3]) || p[3] == '\0' || p[3] == '[')) {
            p += 3;
            int len = 0;
            tokens[i].line = line;
            while (isspace(*p)) {
                p++;
            }

            if (*p == '[') {
                p++;
                if (isdigit(*p)) {
                    tokens[i].length = strtol(p, &p, 10);
                }
                if (*p == ']') p++;
                tokens[i].type = TY_INT;
                tokens[i].kind = TK_ARRAY;
            } else {
                tokens[i].kind = TK_INT;
            }
            ensure_token_capacity();

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i].line = line;
            tokens[i].kind = TK_IDENT;
            ensure_token_capacity();
            continue;
        }

        if (strncmp(p, "str", 3) == 0 && (isspace(p[3]) || p[3] == '\0' || p[3] == '[')) {
            p += 3;
            int len = 0;
            tokens[i].line = line;
            

            while (isspace(*p)) {
                p++;
            }

            if (*p == '[') {
                p++;
                if (isdigit(*p)) {
                    tokens[i].length = strtol(p, &p, 10);
                }
                if (*p == ']') p++;
                tokens[i].type = TY_STRING;
                tokens[i].kind = TK_ARRAY;
            } else {
                tokens[i].kind = TK_STRING_TYPE;
            }
            ensure_token_capacity();

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i].line = line;
            tokens[i].kind = TK_IDENT;
            ensure_token_capacity();
            continue;
        }

        if (strncmp(p, "bool", 4) == 0 && (isspace(p[4]) || p[4] == '\0' || p[4] == '[')) {
            p += 4;
            int len = 0;
            tokens[i].line = line;

            while (isspace(*p)) {
                p++;
            }

            if (*p == '[') {
                p++;
                if (isdigit(*p)) {
                    tokens[i].length = strtol(p, &p, 10);
                }
                if (*p == ']') p++;
                tokens[i].type = TY_BOOL;
                tokens[i].kind = TK_ARRAY;
            } else {
                tokens[i].kind = TK_BOOL_TYPE;
            }
            ensure_token_capacity();

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i].line = line;
            tokens[i].kind = TK_IDENT;
            ensure_token_capacity();
            continue;
        }

        if (strncmp(p, "true", 4) == 0 && (isspace(p[4]) || p[4] == '\0' || p[4] == ';' || p[4] == ')')) {
            tokens[i].kind = TK_BOOL;
            tokens[i].bool_val = true;
            tokens[i].line = line;
            ensure_token_capacity();
            p += 4;
            continue;
        }

        if (strncmp(p, "false", 5) == 0 && (isspace(p[5]) || p[5] == '\0' || p[5] == ';' || p[5] == ')')) {
            tokens[i].kind = TK_BOOL;
            tokens[i].bool_val = false;
            tokens[i].line = line;
            ensure_token_capacity();
            p += 5;
            continue;
        }

        if (*p == '(') {
            p = scan_keyword(p, TK_LPAREN, 1);
            continue;
        }

        if (*p == ')') {
            p = scan_keyword(p, TK_RPAREN, 1);
            continue;
        }

        if (*p == '{') {
            p = scan_keyword(p, TK_LBRACE, 1);
            continue;
        }

        if (*p == '}') {
            p = scan_keyword(p, TK_RBRACE, 1);
            continue;
        }

        if (*p == '+') {
            p++;
            if (*p == '+') {
                p = scan_keyword(p, TK_INC, 1);
            } else {
                p = scan_keyword(p, TK_PLUS, 0);
            }
            continue;
        }

        if (*p == '-') {
            p = scan_keyword(p, TK_MINUS, 1);
            continue;
        }

        if (*p == '*') {
            p = scan_keyword(p, TK_MUL, 1);
            continue;
        }

        if (*p == '/') {
            p = scan_keyword(p, TK_DIV, 1);
            continue;
        }

        if (*p == '%') {
            p = scan_keyword(p, TK_MOD, 1);
            continue;
        }

        if (*p == ';') {
            p = scan_keyword(p, TK_SEMI, 1);
            continue;
        }

        if (*p == '<') {
            p++;
            if (*p == '=') {
                p = scan_keyword(p, TK_LE, 1);
            } else {
                p = scan_keyword(p, TK_LT, 0);
            }
            continue;
        }

        if (*p == '>') {
            p++;
            if (*p == '=') {
                p = scan_keyword(p, TK_GE, 1);
            } else {
                p = scan_keyword(p, TK_GT, 0);
            }
            continue;
        }

        if (*p == '[') {
            p = scan_keyword(p, TK_LBRACKET, 1);
            continue;
        }

        if (*p == ']') {
            p = scan_keyword(p, TK_RBRACKET, 1);
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
            tokens[i].length = len;
            tokens[i].line = line;
            p++;
            ensure_token_capacity();
            continue;
        }

        if (isalpha(*p) || *p == '_') {
            int len = 0;
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i].line = line;

            if (*p == ':') {
                p = scan_keyword(p, TK_IDENT, 0);
                p = scan_keyword(p, TK_COLON, 1);
            } else {
                p = scan_keyword(p, TK_IDENT, 0);
            }
            continue;
        }

        if (*p == '!') {
            p++;
            if (*p == '=') {
                p = scan_keyword(p, TK_NE, 1);
            }
            continue;
        }

        if (*p == ',') {
            p++;
            continue;
        }

        if (*p == '=') {
            p++;
            if (*p == '=') {
                p = scan_keyword(p, TK_EQ, 1);
            } else {
                p = scan_keyword(p, TK_ASSIGN, 0);
            }
            continue;
        }

        printf("Line %d: Unknown character '%c'\n", line, *p);
        exit(1);
    }
    tokens[i].kind = TK_EOF;
    tokens[i].line = line;

    if (g_debug_token) debug_token(i);
}
