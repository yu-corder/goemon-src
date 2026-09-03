#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "token.h"
#include "debug.h"
int pos = 0;

static Node* parse_statement();
static Node* parse_evaluation();
static Node* parse_expression();

static Token* next_token() {
    return &tokens[pos++];
}

static Token* prev_token() {
    return &tokens[pos--];
}

static TypeKind token_convert_type(TokenKind kind) {
    switch (kind) {
        case TK_INT:
            return TY_INT;
        case TK_STRING_TYPE:
            return TY_STRING;
        case TK_BOOL_TYPE:
            return TY_BOOL;
        case TK_VOID:
            return TY_VOID;
        default:
            fprintf(stderr,
                "Expected Type but got %s\n",
                token_name(kind));
            exit(1);
    }
}

static TypeKind ret_type() {
    switch (tokens[pos].kind) {
        case TK_INT:
        case TK_STRING_TYPE:
        case TK_BOOL_TYPE:
        case TK_VOID: {
            Token *t = next_token();
            return token_convert_type(t->kind);
        }
        case TK_IDENT:
            return TY_VOID;
        default:
            fprintf(stderr,
                "Expected TK_IDENT but got %s\n",
                token_name(tokens[pos].kind));
            exit(1);
    }
}

static bool consume(TokenKind kind) {
    if (tokens[pos].kind != kind) {
        return false;
    }

    next_token();
    return true;
}

static Token *expect_ident() {
    if (tokens[pos].kind != TK_IDENT) {
        fprintf(stderr,
            "Expected TK_IDENT but got %s\n",
            token_name(tokens[pos].kind));
        exit(1);
    }

    Token *tok = next_token();
    return tok;
}

static void expect(TokenKind kind) {
    if (!consume(kind)) {
        fprintf(stderr,
            "Expected %s but got %s\n",
            token_name(kind),
            token_name(tokens[pos].kind));
        exit(1);
    }
}

static Node* parse_statement_list(TokenKind kind) {
    Node *stmt = NULL;
    Node *head = NULL;
    Node *tail = NULL;
    while (tokens[pos].kind != kind && tokens[pos].kind != TK_EOF) {
        stmt = parse_statement();

        if (!stmt) continue;

        if (!head) {
            head = stmt;
            tail = stmt;
        } else {
            tail->next = stmt;
            tail = stmt;
        }
    }

    return head;
}

static Node* parse_argument_list(TokenKind kind) {
    Node *stmt = NULL;
    Node *head = NULL;
    Node *tail = NULL;

    while (tokens[pos].kind != kind && tokens[pos].kind != TK_EOF) {
        stmt = parse_evaluation();

        if (!stmt) continue;

        if (!head) {
            head = stmt;
            tail = stmt;
        } else {
            tail->next = stmt;
            tail = stmt;
        }
    }

    return head;
}


static Node* parse_binary(Node *lhs, NodeKind kind) {
    next_token();
    Node *rhs = parse_expression();
    return new_binary_node(kind, lhs, rhs);
}

static Node* parse_primary() {
    if (tokens[pos].kind == TK_PLUS) {
        next_token();
    }

    Token *t = next_token();
    Node *node = NULL;
    if (t->kind == TK_LPAREN) {
        node = parse_expression();
        next_token();
        return node;
    }
    
    if (t->kind == TK_NUMBER) {
        node = new_num_node(&t->val);
    } else if (t->kind == TK_IDENT) {
        if (tokens[pos].kind == TK_LPAREN) {
            next_token();
            Node *arg_head = parse_argument_list(TK_RPAREN);
            if (tokens[pos].kind == TK_RPAREN) next_token();
            node =  new_call_node(ND_CALL, t->str, arg_head);
        } else if (tokens[pos].kind == TK_INC) {
            next_token();
            Node *var = new_var_node(t->str);
            node = new_unary_node(ND_INC, var);
        } else {
            node = new_var_node(t->str);
        }
    } else if (t->kind == TK_STRING) {
        node = new_str_node(t->str, &t->length);
    } else if (t->kind == TK_BOOL) {
        node = new_bool_node(t->bool_val);
    }
    return node;
}

static Node* parse_term() {
    Node *node = parse_primary();

    while(tokens[pos].kind == TK_MUL || tokens[pos].kind == TK_DIV || tokens[pos].kind == TK_MOD) {
        TokenKind kind_type = tokens[pos].kind;
        next_token();
        Node *rhs = parse_primary();

        if (kind_type == TK_MUL) {
            node = new_binary_node(ND_MUL, node, rhs);
        } else if (kind_type == TK_DIV) {
            node = new_binary_node(ND_DIV, node, rhs);
        } else if (kind_type == TK_MOD) {
            node = new_binary_node(ND_MOD, node, rhs);
        }
    }
    return node;
}

static Node* parse_evaluation() {
    Node *node = parse_expression();

    if (tokens[pos].kind == TK_LT) {
        node = parse_binary(node, ND_LT);
    } else if (tokens[pos].kind == TK_GT) {
        node = parse_binary(node, ND_GT);
    } else if (tokens[pos].kind == TK_LE) {
        node = parse_binary(node, ND_LE);
    } else if (tokens[pos].kind == TK_GE) {
        node = parse_binary(node, ND_GE);
    } else if (tokens[pos].kind == TK_EQ) {
        node = parse_binary(node, ND_EQ);
    } else if (tokens[pos].kind == TK_NE) {
        node = parse_binary(node, ND_NE);
    }

    return node;
}

static Node* parse_expression() {
    Node *node = parse_term();
    while (tokens[pos].kind == TK_PLUS || tokens[pos].kind == TK_MINUS) {
        TokenKind kind_type = tokens[pos].kind;
        next_token();
        Node *rhs = parse_term();

        if (kind_type == TK_PLUS) {
            node = new_binary_node(ND_ADD, node, rhs);
        } else if (kind_type == TK_MINUS) {
            node = new_binary_node(ND_MINUS, node, rhs);
        }
    }
    return node;
}

static Node* parse_function() {
    TypeKind ret_kind = ret_type();

    Token *ident = expect_ident();

    consume(TK_LPAREN);

    Node *param_head = parse_statement_list(TK_RPAREN);

    consume(TK_RPAREN);

    consume(TK_LBRACE);

    Node *body_head = parse_statement_list(TK_RBRACE);

    consume(TK_RBRACE);

    return new_func_node(ND_FUNCTION, ident->str, param_head, body_head, ret_kind);
}

static Node* parse_if () {
    consume(TK_LPAREN);
    Node *condition = parse_evaluation();

    consume(TK_RPAREN);

    consume(TK_LBRACE);

    Node *then_stmt = NULL;
    Node *then_head = NULL;
    Node *then_tail = NULL;

    while (tokens[pos].kind != TK_RBRACE && tokens[pos].kind != TK_EOF) {
        then_stmt = parse_statement();
        if (!then_stmt) continue;

        if (!then_head) {
            then_head = then_stmt;
            then_tail = then_stmt;
        } else {
            then_tail->next = then_stmt;
            then_tail = then_stmt;
        }
    }

    consume(TK_RBRACE);

    Node *else_head = NULL;
    Node *else_tail = NULL;
    Node *else_stmt = NULL;
    if (consume(TK_ELSE)) {
        if (consume(TK_IF)) {
            else_head = parse_if();
        } else {
            consume(TK_LBRACE);
            while (tokens[pos].kind != TK_RBRACE && tokens[pos].kind != TK_EOF) {
                else_stmt = parse_statement();

                if (!else_stmt) continue;

                if (!else_head) {
                    else_head = else_stmt;
                    else_tail = else_stmt;
                } else {
                    else_tail->next = else_stmt;
                    else_tail = else_stmt;
                }
            }

            consume(TK_RBRACE);
        }

    }

    return new_if_node(ND_IF, condition, then_head, else_head);
}

static Node* parse_while() {
    consume(TK_LPAREN);

    Node *condition = parse_evaluation();

    consume(TK_RPAREN);

    consume(TK_LBRACE);
    Node *body_head = parse_statement_list(TK_RBRACE);

    consume(TK_RBRACE);

    return new_loop_node(ND_WHILE, condition, body_head);
}

static Node* parse_for() {
    consume(TK_LPAREN);

    Node *init = NULL;
    Node *var = NULL;

    consume(TK_INT);
    
    Token *t = expect_ident();
    if (consume(TK_ASSIGN)) {
        init = parse_evaluation();
        expect(TK_SEMI);
        var = new_var_node(t->str);
    }
    init = new_decl_node(ND_VAR_DECL, var, init, TY_INT);

    Node *condition = NULL;
    if (tokens[pos].kind != TK_SEMI) {
        condition = parse_evaluation();
    }
    consume(TK_SEMI);

    Node *update = NULL;
    Token *upd_t = expect_ident();
    if (consume(TK_INC)) {
        Node *var = new_var_node(upd_t->str);
        update = new_unary_node(ND_INC, var);
    }

    consume(TK_RPAREN);

    consume(TK_LBRACE);

    Node *body_head = NULL;
    Node *body_tail = NULL;
    Node *body_stmt = NULL;
    while (tokens[pos].kind != TK_RBRACE && tokens[pos].kind != TK_EOF) {
        body_stmt = parse_statement();

        if (!body_stmt) continue;

        if (!body_head) {
            body_head = body_stmt;
            body_tail = body_stmt;
        } else {
            body_tail->next = body_stmt;
            body_tail = body_stmt;
        }
    }

    consume(TK_RBRACE);
    return new_for_node(ND_FOR, init, condition, update, body_head);
}

static Node* parse_statement() {
    Token *t = next_token();
    switch(t->kind) {
        case TK_PRINT: {
            Node *rhs = parse_evaluation();
            return new_unary_node(ND_PRINT, rhs);
        }
        case TK_INT: {
            Token *ident = expect_ident();
            Node *lhs = new_var_node(ident->str);
            
            if (consume(TK_ASSIGN)) {
                Node *rhs = parse_evaluation();
                expect(TK_SEMI);
                return new_decl_node(ND_VAR_DECL, lhs, rhs, TY_INT);
            } else {
                return new_decl_no_assignment_node(ND_VAR_DECL, lhs, TY_INT);
            }
        }
        case TK_STRING_TYPE: {
            Token *ident = expect_ident();
            Node *lhs = new_var_node(ident->str);

            if (consume(TK_ASSIGN)) {
                Node *rhs = parse_evaluation();
                expect(TK_SEMI);
                return new_decl_node(ND_VAR_DECL, lhs, rhs, TY_STRING);
            } else {
                return new_decl_no_assignment_node(ND_VAR_DECL, lhs, TY_STRING);
            }
        }
        case TK_BOOL_TYPE: {
            Token *ident = expect_ident();
            Node *lhs = new_var_node(ident->str);

            if (consume(TK_ASSIGN)) {
                Node *rhs = parse_evaluation();
                expect(TK_SEMI);
                return new_decl_node(ND_VAR_DECL, lhs, rhs, TY_BOOL);
            } else {
                return new_decl_no_assignment_node(ND_VAR_DECL, lhs, TY_BOOL);
            }
        }
        case TK_IDENT: {
            consume(TK_COLON);

            Node *lhs = new_var_node(t->str);
            if (consume(TK_ASSIGN)) {
                Node *rhs = parse_evaluation();
                expect(TK_SEMI);
                return new_binary_node(ND_ASSIGN, lhs, rhs);
            } else {
                prev_token();
                return parse_evaluation();
            }
        }
        case TK_IF: {
            return parse_if();
        }
        case TK_WHILE: {
            return parse_while();
        }
        case TK_BREAK: {
            return new_simple_node(ND_BREAK);
        }
        case TK_CONTINUE: {
            return new_simple_node(ND_CONTINUE);
        }
        case TK_FOR: {
            return parse_for();
        }
        case TK_FUNCTION: {
            return parse_function();
        }
        case TK_RET: {
            Node *rhs = parse_evaluation();
            return new_unary_node(ND_RET, rhs);
        }
        default:
            return NULL;
    }
}

Node* parse_program () {
    pos = 0;

    Node *head = NULL;
    Node *tail = NULL;
    while (tokens[pos].kind != TK_EOF) {
        Node *stmt = parse_statement();

        if (!stmt) continue;

        if (!head) {
            head = stmt;
            tail = stmt;
        } else {
            tail->next = stmt;
            tail = stmt;
        }
    }

    return head;
}
