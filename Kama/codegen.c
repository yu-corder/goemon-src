#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolver.h"
#include "codegen.h"

String string_table[128];
int string_count = 0;

typedef struct {
    int breaks[128];
    int break_count;
    int continue_target;
} LoopContext;

int bytecode[1024];
LoopContext loop_stack[128];
int loop_depth = 0;
int count = 0;

void emit_no_operand(OpCode op_code) {
    bytecode[count++] = op_code;
}

void emit_one_operand (OpCode op_code, int *val) {
    bytecode[count++] = op_code;
    if (val != NULL) {
        bytecode[count++] = *val;
    }
}

void emit_two_operand(OpCode op_code, int *val1, int *val2) {
    bytecode[count++] = op_code;
    bytecode[count++] = *val1;
    bytecode[count++] = *val2;
}


void generate_binary(Node *node, OpCode op) {
    generate(node->lhs);
    generate(node->rhs);
    emit_no_operand(op);
}

typedef struct {
    char name[32];
    int address;
    int depth;
} ParamsTmp;

void generate(Node *node) {
    if (node == NULL) return;
    while (node) {

        switch (node->kind) {
            case ND_NUM: {
                emit_one_operand(OP_PUSH, &node->val);
                break;
            }
            case ND_STR: {
                string_table[string_count].length = node->len;
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
                if (node->lhs->kind != ND_STR && node->lhs->type != TY_STRING) {
                    emit_no_operand(OP_PRINT);
                } else {
                    emit_no_operand(OP_PRINT_STRING);
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
                enter_scope();
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

                leave_scope();
                break;
            }
            case ND_WHILE: {
                enter_scope();

                loop_depth++;
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
                leave_scope();
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
                enter_scope();

                loop_depth++;
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

                leave_scope();
                break;
            }
            case ND_FUNCTION: {
                enter_scope();
                
                int my_jmp_idx = count;
                int zero = 0;
                emit_one_operand(OP_JMP, &zero);

                Node *params = node->params;
                ParamsTmp params_tmp_table[128];
                int p_count = 0;
                while (params) {
                    params_tmp_table[p_count].address = params->lhs->address;
                    params_tmp_table[p_count].depth = params->lhs->depth;
                    strcpy(params_tmp_table[p_count].name, params->lhs->name);
                    p_count++;
                    params = params->next;
                }
                
                for (int i = p_count - 1; i >= 0; i--) {
                    emit_two_operand(OP_STORE_LOCAL, &params_tmp_table[i].address, &params_tmp_table[i].depth);
                }

                generate(node->body);

                emit_no_operand(OP_RET);

                bytecode[my_jmp_idx + 1] = count;
                leave_scope();
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
