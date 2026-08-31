#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

extern int g_debug_token;
extern void debug_token(int i);

int line = 1;
void tokenize (char *p) {
    int i = 0;
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
            i++;
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

        if (strncmp(p, "while", 5) == 0 && (isspace(p[5]) || p[5] == '\0')) {
            tokens[i++].kind = TK_WHILE;
            p += 5;
            continue;
        }

        if (strncmp(p, "break", 5) == 0 && (isspace(p[5]) || p[5] == '\0' || p[5] == ';')) {
            tokens[i++].kind = TK_BREAK;
            p += 5;
            continue;
        }

        if (strncmp(p, "continue", 8) == 0 && (isspace(p[8]) || p[8] == '\0' || p[8] == ';')) {
            tokens[i++].kind = TK_CONTINUE;
            p += 8;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            tokens[i++].kind = TK_FOR;
            p += 3;
            continue;
        }

        if (strncmp(p, "function", 8) == 0 && (isspace(p[8]) || p[8] == '\0')) {
            tokens[i++].kind = TK_FUNCTION;
            p += 8;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && (isspace(p[6]) || p[6] == '\0')) {
            tokens[i++].kind = TK_RET;
            p += 6;
            continue;
        }

        if (strncmp(p, "int", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            p += 3;
            int len = 0;
            tokens[i++].kind = TK_INT;

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i++].kind = TK_IDENT;
            continue;
        }

        if (strncmp(p, "str", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            p += 3;
            int len = 0;
            tokens[i++].kind = TK_STRING_TYPE;

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i++].kind = TK_IDENT;
            continue;
        }

        if (strncmp(p, "bool", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            p += 4;
            int len = 0;
            tokens[i++].kind = TK_BOOL_TYPE;

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i++].kind = TK_IDENT;
            continue;
        }

        if (strncmp(p, "true", 4) == 0 && (isspace(p[4]) || p[4] == '\0' || p[4] == ';' || p[4] == ')')) {
            tokens[i].kind = TK_BOOL;
            tokens[i].bool_val = true;
            i++;
            p += 4;
            continue;
        }

        if (strncmp(p, "false", 5) == 0 && (isspace(p[5]) || p[5] == '\0' || p[5] == ';' || p[5] == ')')) {
            tokens[i].kind = TK_BOOL;
            tokens[i].bool_val = false;
            i++;
            p += 5;
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
            p++;
            if (*p == '+') {
                tokens[i++].kind = TK_INC;
                p++;
            } else {
                tokens[i++].kind = TK_PLUS;
            }
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
            tokens[i].length = len;
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

        if (*p == ',') {
            p++;
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

        printf("Line %d: Unknown character '%c'\n", line, *p);
        exit(1);
    }
    tokens[i].kind = TK_EOF;

    if (g_debug_token) debug_token(i);
}
