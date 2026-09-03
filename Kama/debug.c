#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

bool g_debug_ast = false;
bool g_debug_token = false;
bool g_debug_binary = false;

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

const char* type_name(TypeKind kind) {
    switch(kind) {
        case TY_INT: return "INT";
        case TY_STRING: return "STRING";
        case TY_BOOL: return "BOOL";
        default: return "UNKNOWN";
    }
}

static void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

void debug_ast_node(Node *node, int depth) {
    if (node == NULL) return;


    while (node) {
        print_indent(depth);

        printf("[%s]",
            node->kind == ND_NUM ? "NUM" :
            node->kind == ND_STR ? "STR" :
            node->kind == ND_ADD ? "ADD" :
            node->kind == ND_MINUS ? "MINUS" :
            node->kind == ND_MUL ? "MUL" :
            node->kind == ND_DIV  ? "DIV" :
            node->kind == ND_MOD ? "MOD" :
            node->kind == ND_LT ? "LT" :
            node->kind == ND_GT ? "GT" :
            node->kind == ND_LE ? "LE" :
            node->kind == ND_GE ? "GE" :
            node->kind == ND_EQ ? "EQ" :
            node->kind == ND_NE ? "NE" :
            node->kind == ND_VAR_DECL ? "DECL" :
            node->kind == ND_ASSIGN ? "ASSIGN" :
            node->kind == ND_VAR ? "VAR" :
            node->kind == ND_PRINT ? "PRINT" :
            node->kind == ND_IF ? "IF" :
            node->kind == ND_INC ? "INC" :
            node->kind == ND_WHILE ? "WHILE" :
            node->kind == ND_BREAK ? "BREAK" :
            node->kind == ND_CONTINUE ? "CONTINUE" :
            node->kind == ND_FOR ? "FOR" :
            node->kind == ND_FUNCTION ? "FUNCTION" :
            node->kind == ND_CALL ? "CALL" :
            node->kind == ND_RET ? "RET" :
            node->kind == ND_BOOL ? "BOOL" :
            "UNKNOWN"
        );


        if (node->type != 0) printf("(%s)", type_name(node->type));
        

        if (node->kind == ND_VAR) {
            printf("(%s)", node->name);
            printf("(address=%d)", node->address);
            if (!node->is_global) {
                printf("(depth=%d)", node->depth);
            }
        }

        if (node->kind == ND_CALL) {
            printf("(address=%d)", node->address);
        }

        if (node->kind == ND_NUM) {
            printf(" val=%d", node->val);
        }

        if (node->kind == ND_BOOL) {
            printf(" val=%s", node->bool_val ? "true" : "false");
        }

        if (node->kind == ND_STR) {
            printf(" val=%s", node->str);
        }

        printf("\n");

        if (node->kind == ND_IF) {
            print_indent(depth + 1);
            printf("[CONDITION]\n");
            debug_ast_node(node->condition, depth + 2);

            print_indent(depth + 1);
            printf("[THEN]\n");
            debug_ast_node(node->body, depth + 2);

            if (node->else_stmt) {
                print_indent(depth + 1);
                printf("[ELSE]\n");
                debug_ast_node(node->else_stmt, depth + 2);
            }
        } else if (node->kind == ND_WHILE) {
            print_indent(depth + 1);
            printf("[CONDITION]\n");
            debug_ast_node(node->condition, depth + 2);

            print_indent(depth + 1);
            printf("[BODY]\n");
            debug_ast_node(node->body, depth + 2);
        } else if (node->kind == ND_FOR) {
            print_indent(depth + 1);
            printf("[INIT]\n");
            debug_ast_node(node->init, depth + 2);

            print_indent(depth + 1);
            printf("[CONDITION]\n");
            debug_ast_node(node->condition, depth + 2);

            print_indent(depth + 1);
            printf("[UPDATE]\n");
            debug_ast_node(node->update, depth + 2);

            print_indent(depth + 1);
            printf("[BODY]\n");
            debug_ast_node(node->body, depth + 2);
        } else if (node->kind == ND_FUNCTION) {
            print_indent(depth + 1);
            printf("[NAME]\n");
            print_indent(depth + 2);
            printf("(%s)\n", node->func_name);

            print_indent(depth + 1);
            printf("[PARAMS]\n");
            debug_ast_node(node->params, depth + 2);

            print_indent(depth + 1);
            printf("[BODY]\n");
            debug_ast_node(node->body, depth + 2);
        } else if (node->kind == ND_CALL) {
            print_indent(depth + 1);
            printf("[NAME]\n");
            print_indent(depth + 2);
            printf("(%s)\n", node->func_name);

            print_indent(depth + 1);
            printf("[PARAMS]\n");
            debug_ast_node(node->params, depth + 2);

        } else {
            debug_ast_node(node->lhs, depth + 1);
            debug_ast_node(node->rhs, depth + 1);
        }
        node = node->next;
    }
}

