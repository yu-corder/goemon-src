#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolver.h"
#include "codegen.h"
#include "type.h"
#include "kama_string.h"
#include "bytecode.h"
#include "ast.h"

typedef struct {
    int *breaks;
    int break_count;
    int break_capacity;
    int continue_target;
} LoopContext;

static LoopContext *loop_stack;
static int loop_depth = 0;
static int loop_stack_capacity;

static void generate(Node *node);

static void free_loop_stack(void) {
    for (int i = 0; i < loop_stack_capacity; i++) {
        free(loop_stack[i].breaks);
        loop_stack[i].breaks = NULL;
    }
    free(loop_stack);
    loop_stack = NULL;
    loop_stack_capacity = 0;
    loop_depth = 0;
}

void init_loop_stack(void) {
    loop_stack_capacity = 8;
    loop_stack = malloc(sizeof(LoopContext) * loop_stack_capacity);

    if (loop_stack == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to allocate loop stack\n");
        exit(1);
    }

    for (int i = 0; i < loop_stack_capacity; i++) {
        loop_stack[i].break_capacity = 8;
        loop_stack[i].break_count = 0;
        loop_stack[i].continue_target = -1;
        int *breaks = malloc(sizeof(int) * loop_stack[i].break_capacity);

        if (breaks == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate breaks\n");
            exit(1);
        }

        loop_stack[i].breaks = breaks;
    }
}

static void make_loop_stack_bigger(void) {
    int old_capacity = loop_stack_capacity;
    int new_capacity = loop_stack_capacity * 2;
    LoopContext *new_loop_stack = realloc(loop_stack, sizeof(LoopContext) * new_capacity);

    if (new_loop_stack ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize loop stack\n");
        exit(1);
    }

    loop_stack = new_loop_stack;

    for (int i = old_capacity; i < new_capacity; i++) {
        loop_stack[i].break_capacity = 8;
        loop_stack[i].break_count = 0;
        loop_stack[i].continue_target = -1;
        int *breaks = malloc(sizeof(int) * loop_stack[i].break_capacity);

        if (breaks == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to allocate breaks\n");
            exit(1);
        }
        loop_stack[i].breaks = breaks;
    }

    loop_stack_capacity = new_capacity;
}

static void make_loop_stack_breaks_bigger(void) {
    int new_capacity = loop_stack[loop_depth].break_capacity * 2;
    int *new_breaks = realloc(
        loop_stack[loop_depth].breaks,
        sizeof(int) * new_capacity
    );

    if (new_breaks ==  NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize breaks\n");
        exit(1);
    }

    loop_stack[loop_depth].breaks = new_breaks;
    loop_stack[loop_depth].break_capacity = new_capacity;
}


static void generate_binary(Node *node, OpCode op) {
    generate(node->lhs);
    generate(node->rhs);
    emit_no_operand(op);
}

typedef struct {
    int address;
    int depth;
} ParamsTmp;

static void generate(Node *node) {
    if (node == NULL) return;
    while (node) {

        switch (node->kind) {
            case ND_NUM: {
                emit_one_operand(OP_PUSH, &node->val);
                break;
            }
            case ND_STR: {
                while (string_count >= string_capacity) {
                    make_bigger();
                }
                string_table[string_count].length = node->len;
                string_table[string_count].str = malloc(sizeof(char) * node->len + 1);
                if (string_table[string_count].str == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(1);
                }

                strcpy(string_table[string_count].str, node->str);
                node->address = string_count;
                emit_one_operand(OP_PUSH, &node->address);
                string_count++;
                break;
            }
            case ND_BOOL: {
                emit_one_operand(OP_PUSH, &node->val);
                break;
            }
            case ND_ARRAY_DECL: {
                emit_two_operand_type(OP_MAKE_ARRAY, &node->len, node->type);
                if (node->lhs->is_global) {
                    emit_one_operand(OP_STORE, &node->lhs->address);
                } else {
                    emit_two_operand(OP_STORE_LOCAL, &node->lhs->address, &node->lhs->depth);
                }
                break;
            }
            case ND_ARRAY: {
                generate(node->rhs);
                if (node->is_global) {
                    emit_one_operand(OP_LOAD, &node->address);
                } else {
                    emit_two_operand(OP_LOAD_LOCAL, &node->address, &node->depth);
                }
                emit_no_operand(OP_ARRAY_LOAD);
                break;
            }
            case ND_VAR_DECL: {
                if (node->rhs != NULL) {
                    generate(node->rhs);
                    
                    if (node->lhs->is_global) {
                        emit_one_operand(OP_STORE, &node->lhs->address);
                    } else {
                        emit_two_operand(OP_STORE_LOCAL, &node->lhs->address, &node->lhs->depth);
                    }
                }
                break;
            }
            case ND_ASSIGN: {
                generate(node->rhs);
                if (node->lhs->is_global) {
                    emit_one_operand(OP_STORE, &node->lhs->address);
                } else {
                    emit_two_operand(OP_STORE_LOCAL, &node->lhs->address, &node->lhs->depth);
                }
                break;
            }
            case ND_ASSIGN_ARRAY: {
                generate(node->rhs);
                generate(node->lhs);
                if (node->lhs->is_global) {
                    emit_one_operand(OP_ARRAY_STORE, &node->lhs->address);
                } else {
                    emit_two_operand(OP_ARRAY_STORE_LOCAL, &node->lhs->address, &node->lhs->depth);
                }
                break;
            }
            case ND_ARRAY_STORE: {
                generate(node->rhs);
                break;
            }
            case ND_VAR: {
                if (node->is_global) {
                    emit_one_operand(OP_LOAD, &node->address);
                } else {
                    emit_two_operand(OP_LOAD_LOCAL, &node->address, &node->depth);
                }
                break;
            }
            case ND_PRINT: {
                generate(node->lhs);
                if (node->lhs->type == TY_STRING || node->lhs->kind == ND_STR) {
                    emit_no_operand(OP_PRINT_STRING);
                } else if (node->lhs->type == TY_BOOL || node->lhs->kind == ND_BOOL) {
                    emit_no_operand(OP_PRINT_BOOL);
                } else {
                    emit_no_operand(OP_PRINT);
                }
                break;
            }
            case ND_ADD: {
                generate_binary(node, OP_ADD);
                break;
            }
            case ND_MINUS: {
                generate_binary(node, OP_SUB);
                break;
            }
            case ND_MUL: {
                generate_binary(node, OP_MUL);
                break;
            }
            case ND_MOD: {
                generate_binary(node, OP_MOD);
                break;
            }
            case ND_DIV: {
                generate_binary(node, OP_DIV);
                break;
            }
            case ND_LT: {
                generate_binary(node, OP_LT);
                break;
            }
            case ND_LE: {
                generate_binary(node, OP_LE);
                break;
            }
            case ND_GT: {
                generate_binary(node, OP_GT);
                break;
            }
            case ND_GE: {
                generate_binary(node, OP_GE);
                break;
            }
            case ND_EQ: {
                generate_binary(node, OP_EQ);
                break;
            }
            case ND_NE: {
                generate_binary(node, OP_NE);
                break;
            }
            case ND_IF: {
                generate(node->condition);

                int my_jz_idx = count;
                int zero = 0;
                emit_one_operand(OP_JZ, &zero);

                generate(node->body);

                if (node->else_stmt) {
                    int my_jmp_idx = count;
                    emit_one_operand(OP_JMP, &zero);
                    bytecode[my_jz_idx + 1] = count;

                    generate(node->else_stmt);
                    bytecode[my_jmp_idx + 1] = count;
                } else {
                    bytecode[my_jz_idx + 1] = count;
                }
                break;
            }
            case ND_WHILE: {
                loop_depth++;
                while (loop_depth >= loop_stack_capacity) {
                    make_loop_stack_bigger();
                }
                loop_stack[loop_depth].break_count = 0;

                int my_jmp_idx = count;
                loop_stack[loop_depth].continue_target = my_jmp_idx;
                generate(node->condition);

                int my_jz_idx = count;
                int zero = 0;
                emit_one_operand(OP_JZ, &zero);

                generate(node->body);

                emit_one_operand(OP_JMP, &my_jmp_idx);
                bytecode[my_jz_idx + 1] = count;
                
                for (int i = 0; i < loop_stack[loop_depth].break_count; i++) {
                    int break_jz_idx = loop_stack[loop_depth].breaks[i];
                    bytecode[break_jz_idx + 1] = count;
                }
                loop_depth--;
                break;
            }
            case ND_INC: {
                if (node->lhs->is_global) {
                    emit_one_operand(OP_INC, &node->lhs->address);
                } else {
                    emit_two_operand(OP_INC_LOCAL, &node->lhs->address, &node->lhs->depth);
                }
                break;
            }
            case ND_BREAK: {
                if (loop_depth == 0) {
                    printf("エラー: ループ分の中でしか、breakは使えません。");
                    exit(1);
                }

                while (loop_stack[loop_depth].break_count >= loop_stack[loop_depth].break_capacity) {
                    make_loop_stack_breaks_bigger();
                }

                int index = loop_stack[loop_depth].break_count++;
                loop_stack[loop_depth].breaks[index] = count;
                int zero = 0;
                emit_one_operand(OP_JMP, &zero);
                break;
            }
            case ND_CONTINUE: {
                if (loop_depth == 0) {
                    printf("エラー: ループ分の中でしか、continueは使えません。");
                    exit(1);
                }
                int my_jmp_idx = loop_stack[loop_depth].continue_target;
                emit_one_operand(OP_JMP, &my_jmp_idx);
                break;
            }
            case ND_FOR: {
                loop_depth++;
                while (loop_depth >= loop_stack_capacity) {
                    make_loop_stack_bigger();
                }
                loop_stack[loop_depth].break_count = 0;

                generate(node->init);

                int cond_start_idx = count;

                generate(node->condition);

                int my_jz_idx = count;
                int zero = 0;
                emit_one_operand(OP_JZ, &zero);

                int jump_to_body_idx = count;
                emit_one_operand(OP_JMP, &zero);

                int update_start_idx = count;
                loop_stack[loop_depth].continue_target = update_start_idx;


                generate(node->update);

                emit_one_operand(OP_JMP, &cond_start_idx);
                bytecode[jump_to_body_idx + 1] = count;

                generate(node->body);

                emit_one_operand(OP_JMP, &update_start_idx);
                bytecode[my_jz_idx + 1] = count;

                for (int i = 0; i < loop_stack[loop_depth].break_count; i++) {
                    int break_jz_idx = loop_stack[loop_depth].breaks[i];
                    bytecode[break_jz_idx + 1] = count;
                }
                loop_depth--;
                break;
            }
            case ND_FUNCTION: {
                int my_jmp_idx = count;
                int zero = 0;
                emit_one_operand(OP_JMP, &zero);

                Node *params = node->params;

                int params_tmp_capacity = 8;
                ParamsTmp *params_tmp_table = malloc(sizeof(ParamsTmp) * params_tmp_capacity);
                if (params_tmp_table == NULL) {
                    fprintf(stderr,
                        "Runtime Error: Failed to allocate params tmp table\n");
                    exit(1);
                }

                int p_count = 0;
                while (params) {
                    if (p_count >= params_tmp_capacity) {
                        int new_params_tmp_capacity = params_tmp_capacity * 2;
                        ParamsTmp *new_params_tmp_table = realloc(params_tmp_table, sizeof(ParamsTmp) * new_params_tmp_capacity);
                        if (new_params_tmp_table == NULL) {
                            fprintf(stderr,
                                "Runtime Error: Failed to resize params tmp table\n");
                            exit(1);
                        }
                        params_tmp_capacity = new_params_tmp_capacity;
                        params_tmp_table = new_params_tmp_table;
                    }
                    params_tmp_table[p_count].address = params->lhs->address;
                    params_tmp_table[p_count].depth = params->lhs->depth;
                    p_count++;
                    params = params->next;
                }
                
                for (int i = p_count - 1; i >= 0; i--) {
                    emit_two_operand(OP_STORE_LOCAL, &params_tmp_table[i].address, &params_tmp_table[i].depth);
                }

                free(params_tmp_table);

                generate(node->body);

                emit_no_operand(OP_RET);

                bytecode[my_jmp_idx + 1] = count;
                break;
            }
            case ND_CALL: {
                generate(node->params);
                emit_one_operand(OP_CALL, &node->address);
                break;
            }
            case ND_RET: {
                generate(node->lhs);
                emit_no_operand(OP_RET);
                break;
            }
            default: 
                printf("Unknown node: %d\n", node->kind);
                exit(1);
        }

        node = node->next;
    }
}

void generate_entry(Node *node) {
    init_bytecode();
    init_strings();
    init_loop_stack();
    generate(node);
    emit_no_operand(OP_HALT);
    free_loop_stack();
    free_node_arena();
}
