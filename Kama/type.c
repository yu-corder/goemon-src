#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "symbol.h"
#include "resolver.h"
#include "debug.h"
#include "ast.h"

TypeKind type_check();

void type_check_program(Node *program) {
    while(program) {
        type_check(program);
        program = program->next;
    }

}

void type_check_params(Node *params, char *name) {
    Node *p = params;
    FuncionParamsInfo func_params = find_function_params(name, block_depth);
    for (int i = 0; i < func_params.param_count; i++) {
        TypeKind param = type_check(p);
        if (param != func_params.type[i]) {
            fprintf(stderr,
                "Expecte: %s\n", type_name(func_params.type[i]));
            exit(1);
        }
        p = p->next;
    }
}

TypeKind type_check_expression(Node* node) {
    TypeKind lhs = type_check(node->lhs);
    TypeKind rhs = type_check(node->rhs);

    node->type = TY_INT;
    if (lhs != TY_INT || rhs != TY_INT) {
        fprintf(stderr,
            "Expecte: %s\n", type_name(node->type));
        exit(1);
    }

    return TY_INT;
}

bool type_check_return(Node* node, TypeKind kind) {
    bool has_return = false;
    while (node) {
        switch (node->kind) {
            case ND_IF: {
                type_check_return(node->body, kind);
                break;
            }
            case ND_WHILE: {
                type_check_return(node->body, kind);
                break;
            }
            case ND_FOR: {
                type_check_return(node->body, kind);
                break;
            }
            case ND_RET: {
                if (node->lhs == NULL) {
                    if (kind != TY_VOID) {
                        fprintf(stderr,
                            "Expected: %s\n", type_name(kind));
                        exit(1);
                    }
                    break;
                }

                TypeKind lhs = type_check(node->lhs);
                if (lhs != kind) {
                    fprintf(stderr,
                        "Expected: %s\n", type_name(kind));
                    exit(1);
                }
                has_return = true;
                break;
            }
            default:
                break;
        }
        node = node->next;
    }
    return has_return;
}

TypeKind type_check(Node* node) {
    if (node == NULL) return TY_VOID;
    switch (node->kind) {
        case ND_NUM: {
            return TY_INT;
        }
        case ND_STR: {
            return TY_STRING;
        }
        case ND_BOOL: {
            return TY_BOOL;
        }
        case ND_VAR_DECL: {
            if (node->rhs == NULL) {
                return TY_VOID;
            }

            TypeKind rhs = type_check(node->rhs);
            
            if (rhs != node->type) {
                fprintf(stderr,
                    "Expected: %s\n", type_name(node->type));
                exit(1);
            }

            return TY_VOID;
        }
        case ND_ASSIGN: {
            TypeKind rhs = type_check(node->rhs);
            if (rhs != node->lhs->type) {
                fprintf(stderr,
                    "Expected: %s\n", type_name(node->lhs->type));
                exit(1);
            }

            return TY_VOID;
        }
        case ND_VAR: {
            return node->type;
        }
        case ND_PRINT: {
            type_check(node->lhs);
            return TY_VOID;
        }
        case ND_ADD: {
            return type_check_expression(node);
        }
        case ND_MINUS: {
            return type_check_expression(node);
        }
        case ND_MUL: {
            return type_check_expression(node);
        }
        case ND_MOD: {
            return type_check_expression(node);
        }
        case ND_DIV: {
            return type_check_expression(node);
        }
        case ND_LT: {
            return type_check_expression(node);
        }
        case ND_LE: {
            return type_check_expression(node);
        }
        case ND_GT: {
            return type_check_expression(node);
        }
        case ND_GE: {
            return type_check_expression(node);
        }
        case ND_EQ: {
            return type_check_expression(node);
        }
        case ND_NE: {
            return type_check_expression(node);
        }
        case ND_IF: {
            enter_scope();
            type_check(node->condition);
            type_check_program(node->body);
            leave_scope();
            return TY_VOID;
        }
        case ND_WHILE: {
            enter_scope();
            type_check(node->condition);
            type_check_program(node->body);
            leave_scope();
            return TY_VOID;
        }
        case ND_FOR: {
            enter_scope();
            type_check(node->init);
            type_check(node->condition);
            type_check(node->update);
            type_check_program(node->body);
            leave_scope();
            return TY_VOID;
        }
        case ND_FUNCTION: {
            enter_scope();
            function_count_up();
            bool has_return = type_check_return(node->body, node->type);
            if (node->type != TY_VOID && !has_return) {
                fprintf(stderr,
                    "Expected return statement for function returning %s\n",
                    type_name(node->type));
                exit(1);
            }
            type_check_program(node->body);
            leave_scope();
            return node->type;
        }
        case ND_CALL: {
            type_check_params(node->params, node->func_name);
            FuncionInfo func = find_function(node->func_name, block_depth);
            return func.type;
        }
        case ND_RET: {
            return type_check(node->lhs);
        }
        default: 
        // TODO: implement type check
            return TY_VOID;
    }
}
