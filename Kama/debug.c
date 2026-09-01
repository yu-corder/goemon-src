#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

const char *token_kind_name[] = {
    "TK_INPUT",
    "TK_STORE",
    "TK_LOAD",
    "TK_JZ",
    "TK_HALT",
    "TK_PRINTS",
    "TK_PRINT",
    "TK_NUMBER",
    "TK_STRING",
    "TK_IDENT",
    "TK_COLON",
    "TK_ASSIGN",
    "TK_PLUS",
    "TK_MINUS",
    "TK_MUL",
    "TK_DIV",
    "TK_MOD",
    "TK_SEMI",
    "TK_LT",
    "TK_LE",
    "TK_GT",
    "TK_GE",
    "TK_EQ",
    "TK_NE",
    "TK_IF",
    "TK_ELSE",
    "TK_LPAREN",
    "TK_RPAREN",
    "TK_LBRACE",
    "TK_RBRACE",
    "TK_INC",
    "TK_WHILE",
    "TK_BREAK",
    "TK_CONTINUE",
    "TK_FOR",
    "TK_FUNCTION",
    "TK_CALL",
    "TK_RET",
    "TK_INT",
    "TK_STRING_TYPE",
    "TK_BOOL",
    "TK_BOOL_TYPE",
    "TK_VOID",
    "TK_EOF"
};

const char* token_name(TokenKind kind) {
    return token_kind_name[kind];
}

void debug_token(int count) {
    printf("\n===== TOKEN DUMP =====\n");
    for (int i = 0; i <= count; i++) {

        printf("[%03d] %-12s",
            i,
            token_kind_name[tokens[i].kind]
        );

        if (tokens[i].kind == TK_NUMBER) {
            printf(" value=%d", tokens[i].val);
        }

        if (tokens[i].kind == TK_BOOL) {
            printf(" bool_val=%s", tokens[i].bool_val ? "true" : "false");
        }

        if (
            tokens[i].kind == TK_IDENT ||
            tokens[i].kind == TK_STRING
        ) {
            printf(" text=\"%s\"", tokens[i].str);
        }

        printf("\n");
    }
    printf("======================\n");
}

