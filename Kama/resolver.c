#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolver.h"
#include "token.h"
#include "ast.h"
#include "type.h"
#include "symbol.h"

// =========================
// NAME RESOLUTION
// =========================

int block_depth = 0;
int program_count = 0;
void emit_count_reset() {
    program_count = 0;
}

void emit_count_up() {
    program_count++;
}

void emit_count_two_up() {
    program_count++;
    program_count++;
}

void emit_count_three() {
    program_count++;
    program_count++;
    program_count++;
}

void function_count_up() {
    function_table[block_depth].function_count++;
}

void enter_scope() {
    block_depth++;
    local_scopes[block_depth].variable_count = 0;

    // Nested function definitions are scoped.
    // Reset the next function table when entering a new function scope.
    function_table[block_depth + 1].function_count = 0;
}

void leave_scope() {
    local_scopes[block_depth].variable_count = 0;
    block_depth--;
}

void resolution_variable(Node* node, bool allow_create, TypeKind* type) {
    int addr = -1;
    if (block_depth >= 1) {
        LocalVariablesInfo var = find_local_variable(node->name, block_depth);
        addr = var.address;
        int find_depth = var.depth;
        if (!var.found) {
            GlobalVariablesInfo g_var = find_global_variable(node->name);
            addr = g_var.address;

            if (!g_var.found) {
                if (allow_create) {
                    addr = insert_local_variable(node->name, block_depth, type);
                    var = find_local_variable(node->name, block_depth);

                    node->address = var.address;
                    node->depth = var.depth;
                    node->is_global = false;
                    node->type = var.type;
                    return;
                }

                fprintf(stderr, "Undefined variable: %s\n", node->name);
                exit(1);
            }

            node->address = addr;
            node->is_global = true;
            node->type = g_var.type;
            return;
        } else {
            
            node->address = addr;
            node->depth = find_depth;
            node->is_global = false;
            node->type = var.type;
        }
        
    } else {
        GlobalVariablesInfo var = find_global_variable(node->name);
        if (!var.found) {
            if (allow_create) {
                addr = insert_global_variable(node->name, type);
                var = find_global_variable(node->name);

                node->address = var.address;
                node->is_global = true;
                node->type = var.type;
                return;
            }

            fprintf(stderr, "Undefined variable: %s\n", node->name);
            exit(1);
        }

        node->address = var.address;
        node->is_global = true;
        node->type = var.type;
        return;
    }
}

void name_resolution_binary(Node *node) {
    name_resolution(node->lhs);
    name_resolution(node->rhs);
    emit_count_up();
}

void name_resolution_func(Node *node, int address, int depth) {
    node->address = address;
    node->depth = depth;
}

void param_name_resolution(Node *node, char* name, int address, int depth, TypeKind type) {
    while (node) {
        if (strcmp(node->lhs->name, name) == 0) {
            node->lhs->address = address;
            node->lhs->depth = depth;
            node->lhs->type = type;
            return;
        }
        node = node->next;
    }
}

void name_resolution(Node *node) {
    if (node == NULL) return;
    while (node) {
        switch (node->kind) {
            case ND_NUM: {
                emit_count_two_up();
                break;
            }
            case ND_STR: {
                emit_count_two_up();
                break;
            }
            case ND_BOOL: {
                emit_count_two_up();
                break;
            }
            case ND_VAR_DECL: {
                if (node->rhs != NULL) {
                    name_resolution(node->rhs);
                }
                
                resolution_variable(node->lhs, true, &node->type);

                if (node->lhs->is_global) {
                    emit_count_two_up();
                } else {
                    emit_count_three();
                }
                break;
            }
            case ND_ASSIGN: {
                name_resolution(node->rhs);
                resolution_variable(node->lhs, false, &node->lhs->type);

                if (node->lhs->is_global) {
                    emit_count_two_up();
                } else {
                    emit_count_three();
                }
                break;
            }
            case ND_VAR: {
                resolution_variable(node, false, NULL);

                if (node->is_global) {
                    emit_count_two_up();
                } else {
                    emit_count_three();
                }
                break;
            }
            case ND_PRINT: {
                name_resolution(node->lhs);
                emit_count_up();
                break;
            }
            case ND_ADD: {
                name_resolution_binary(node);
                break;
            }
            case ND_MINUS: {
                name_resolution_binary(node);
                break;
            }
            case ND_MUL: {
                name_resolution_binary(node);
                break;
            }
            case ND_MOD: {
                name_resolution_binary(node);
                break;
            }
            case ND_DIV: {
                name_resolution_binary(node);
                break;
            }
            case ND_LT: {
                name_resolution_binary(node);
                break;
            }
            case ND_LE: {
                name_resolution_binary(node);
                break;
            }
            case ND_GT: {
                name_resolution_binary(node);
                break;
            }
            case ND_GE: {
                name_resolution_binary(node);
                break;
            }
            case ND_EQ: {
                name_resolution_binary(node);
                break;
            }
            case ND_NE: {
                name_resolution_binary(node);
                break;
            }
            case ND_IF: {
                enter_scope();
                name_resolution(node->condition);

                emit_count_two_up();
                name_resolution(node->body);
                if (node->else_stmt) {
                    emit_count_two_up();
                    name_resolution(node->else_stmt);
                }
                leave_scope();
                break;
            }
            case ND_WHILE: {
                enter_scope();
                name_resolution(node->condition);
                emit_count_two_up();

                name_resolution(node->body);
                emit_count_two_up();
                leave_scope();
                break;
            }
            case ND_INC: {
                resolution_variable(node->lhs, false, NULL);

                if (node->lhs->is_global) {
                    emit_count_two_up();
                } else {
                    emit_count_three();
                }
                break;
            }
            case ND_BREAK: {
                emit_count_two_up();
                break;
            }
            case ND_CONTINUE: {
                emit_count_two_up();
                break;
            }
            case ND_FOR: {
                enter_scope();
                name_resolution(node->init);
                name_resolution(node->condition);

                emit_count_two_up();
                emit_count_two_up();
                name_resolution(node->update);
                emit_count_two_up();
                name_resolution(node->body);
                emit_count_two_up();
                leave_scope();
                break;
            }
            case ND_FUNCTION: {
                enter_scope();
                emit_count_two_up();

                int func_start_address = program_count;
                insert_function(node->func_name, func_start_address, block_depth, node->type);

                insert_function_params(node->func_name, node->params, block_depth);
                FuncionParamsInfo func_params = find_function_params(node->func_name, block_depth);
                for (int i = 0; i < func_params.param_count; i++) {
                    LocalVariablesInfo var = find_local_variable(func_params.params[i], block_depth);
                    int addr = var.address;
                    if (!var.found) addr = insert_local_variable(func_params.params[i], block_depth, &func_params.type[i]);
                    var = find_local_variable(func_params.params[i], block_depth);
                    param_name_resolution(node->params, func_params.params[i], var.address, var.depth, var.type);
                    emit_count_three();
                }

                name_resolution(node->body);

                emit_count_up();
                leave_scope();
                break;
            }
            case ND_CALL: {
                FuncionInfo func = find_function(node->func_name, block_depth);
                if (!func.found) {
                    fprintf(stderr, "Undefined function: %s\n", node->func_name);
                    exit(1);
                }
                name_resolution(node->params);
                name_resolution_func(node, func.address, func.depth);
                emit_count_two_up();
                break;
            }
            case ND_RET: {
                name_resolution(node->lhs);
                emit_count_up();
                break;
            }
            default: 
                printf("Unknown node: %d\n", node->kind);
                exit(1);
        }
        node = node->next;
    }
}