#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"
#include "token.h"
#include "debug.h"

Token* next_token();
Token* prev_token();
TypeKind token_convert_type(TokenKind kind);
TypeKind ret_type();
bool consume(TokenKind kind);
Token *expect_ident();
Token *expect_bool();
void expect(TokenKind kind);

Node* parse_statement_list(TokenKind kind);
Node* parse_argument_list(TokenKind kind);
Node* parse_binary();
Node* parse_primary();
Node* parse_term();
Node* parse_evaluation();
Node* parse_expression();
Node* parse_function();
Node* parse_if();
Node* parse_while();
Node* parse_for();
Node* parse_statement();
Node* parse_program();
Node* parse_print();
#endif