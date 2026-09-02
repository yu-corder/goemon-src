#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "token.h"
#include "scanner.h"
#include "debug.h"
#include "type.h"
#include "ast.h"
#include "parser.h"
#include "symbol.h"
#include "resolver.h"

typedef enum { 
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_DUP,
    OP_SWAP,
    OP_POP,
    OP_MOD,
    OP_EQ,
    OP_GE,
    OP_LE,
    OP_NE,
    OP_JMP,
    OP_JZ,
    OP_PRINT,
    OP_STORE,
    OP_STORE_LOCAL,
    OP_LOAD,
    OP_LOAD_LOCAL,
    OP_LT,
    OP_GT,
    OP_INC,
    OP_INC_LOCAL,
    OP_INPUT,
    OP_PRINTS,
    OP_CALL,
    OP_RET,
    OP_PRINT_STRING,
    OP_HALT 
} OpCode;

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t bytecode_size;
    uint32_t string_count;
} GoemonHeader;

typedef struct {
    char str[32];
    int length;
} String;

String string_table[128];
int string_count = 0;

GoemonHeader header();

void generate(Node* node);

TypeKind type_check();
void type_check_program();

const char* type_name(TypeKind type);
const char* token_name(TokenKind kind);

bool g_debug_ast = false;
bool g_debug_token = false;
bool g_debug_binary = false;

void debug_bynary();

int pos = 0;

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

char *read_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror(path); exit(1); }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);
    return buf;
}

int main(int argc, char **argv) {
    int arg = 1;

    while (arg < argc && argv[arg][0] == '-') {
        if (strcmp(argv[arg], "--ast") == 0) {
            g_debug_ast = true;
        } else if (strcmp(argv[arg], "--token") == 0) {
            g_debug_token = true;
        } else if (strcmp(argv[arg], "--binary") == 0) {
            g_debug_binary = true;
        } else {
            printf("Unknown option: %s\n", argv[arg]);
            return 1;
        }

        arg++;
    }

    if (argc - arg < 2) {
        printf("usage: kama-c [options] input.goe output.gb\n");
        printf("  --ast    Print AST\n");
        return 1;
    }

    char *src = read_file(argv[arg]);
    tokenize(src);
    Node *program = parse_program();

    emit_count_reset();
    name_resolution(program);

    if (g_debug_ast) {
        debug_ast_node(program, 1);
    }

    type_check_program(program);
    
    emit_count_reset();
    generate(program);

    emit_no_operand(OP_HALT);

    if (g_debug_binary) {
        debug_bynary();
    }

    GoemonHeader hed = header();

    FILE *dest = fopen(argv[arg + 1], "wb");
    fwrite(&hed, sizeof(GoemonHeader), 1, dest);
    fwrite(bytecode, sizeof(int), count, dest);
    fwrite(string_table, sizeof(String), string_count, dest);
    fclose(dest);

    printf("絶景かな！ Compiled study.goe to study.gb\n");
    return 0;
}


// =========================
// BINARY HEADER
// =========================

GoemonHeader header() {
    GoemonHeader header;
    header.magic = 1.0;
    header.version = 1.0;
    header.bytecode_size = count;
    header.string_count = string_count;

    return header;
}

// =========================
// TYPE CHECK
// =========================
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


// =========================
// GENERATE
// =========================
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

// =========================
// Debug Utilities
// =========================
void debug_bynary() {
    printf("\n===== BINARY DUMP =====\n");
    for (int i = 0; i < count; i++) {
        printf("%d\n", bytecode[i]);
    }
    printf("==========================\n");
}