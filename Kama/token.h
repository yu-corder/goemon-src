#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

#define MAX_TOKENS 4096
typedef enum {
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
    TK_INC,
    TK_WHILE,
    TK_BREAK,
    TK_CONTINUE,
    TK_FOR,
    TK_FUNCTION,
    TK_CALL,
    TK_RET,
    TK_INT,
    TK_STRING_TYPE,
    TK_BOOL,
    TK_BOOL_TYPE,
    TK_VOID,
    TK_EOF,
} TokenKind;

typedef struct {
    TokenKind kind;
    int val;
    char str[256];
    bool bool_val;
    int length;
} Token;
extern Token tokens[MAX_TOKENS];
#endif