#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#define MAX_TOKENS 4096

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
    OP_LOAD,
    OP_LT,
    OP_GT,
    OP_INC,
    OP_INPUT,
    OP_PRINTS,
    OP_HALT 
} OpCode;

typedef enum {
    TK_PUSH,
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
    TK_EOF,
} TokenKind;

typedef enum {
    ND_NUM,
    ND_ADD,
    ND_MINUS,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_LT,
    ND_GT,
    ND_LE,
    ND_GE,
    ND_EQ,
    ND_NE,
    ND_ASSIGN,
    ND_VAR,
    ND_PRINT,
    ND_IF,
    ND_BLOCK,
    ND_INC,
    ND_WHILE,
    ND_BREAK,
    ND_CONTINUE,
    ND_FOR,
} NodeKind;

typedef struct {
    TokenKind kind;
    int val;
    char str[256];
} Token;

typedef struct Node {
    NodeKind kind;

    struct Node *lhs;
    struct Node *rhs;

    struct Node *condition;
    struct Node *body;
    struct Node *else_stmt;

    struct Node *init;
    struct Node *update;
    

    struct Node *next;

    int val;
    char name[32];
} Node;
int node_depth = 0;


typedef struct {
    char name[32];
    int address;
} Label;

typedef struct {
    char name[32];
    int memory_index;
} Variable;

Variable variable_table[128];
int variable_count = 0;

Node node_tree[128];
Node* new_num_node();
Node* new_binary_node();
Node* new_var_node ();
Node* new_simple_node();
Node* new_unary_node();
Node* new_if_node();
Node* new_loop_node();
Node* new_for_node();
void debug_ast_node();
void print_ast();
void generate(Node* node);

Node* parse_evaluation();
Node* parse_expression();
Node* parse_if();
Node* parse_while();
Node* parse_for();

Label symbol_table[128];
int label_count_internal = 0;

bool g_debug_ast = false;
bool g_debug_token = false;
bool g_debug_binary = false;

void debug_token(int count);
void debug_bynary();

int find_label(char *name) {
    for (int i = 0; i < label_count_internal; i++) {
        if (strncmp(symbol_table[i].name, name, strlen(symbol_table[i].name)) == 0) {
            return symbol_table[i].address;
        }
    }
    return -1;
}

int find_variable(char *name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variable_table[i].name, name) == 0) {
            return variable_table[i].memory_index;
        }
    }

    strcpy(variable_table[variable_count].name, name);
    
    if (strncmp(name, "__s", 3) == 0) {
        variable_table[variable_count].memory_index = 1000 + (variable_count * 100);
    } else {
        variable_table[variable_count].memory_index = variable_count;
    }

    return variable_table[variable_count++].memory_index;

}

Token tokens[MAX_TOKENS];
void tokenize (char *p) {
    int i = 0;
    while(*p) {
        if (isspace(*p)) { p++; continue;}

        if (isdigit(*p)) {
            tokens[i].kind = TK_NUMBER;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        if (strncmp(p, "push", 4) == 0 && isspace(p[4])) {
            tokens[i++].kind = TK_PUSH;
            p += 4;
            continue;
        }

        if (strncmp(p, "halt", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            tokens[i++].kind = TK_HALT;
            p += 4;
            continue;
        }

        if (strncmp(p, "print", 5) == 0 && (isspace(p[5]) || p[5] == '\0')) {
            tokens[i++].kind = TK_PRINT;
            p += 5;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && (isspace(p[2]) || p[2] == '\0')) {
            tokens[i++].kind = TK_IF;
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            tokens[i++].kind = TK_ELSE;
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && (isspace(p[5]) || p[5] == '\0')) {
            tokens[i++].kind = TK_WHILE;
            p += 5;
            continue;
        }

        if (strncmp(p, "break", 5) == 0 && (isspace(p[5]) || p[5] == '\0' || p[5] == ';')) {
            tokens[i++].kind = TK_BREAK;
            p += 5;
            continue;
        }

        if (strncmp(p, "continue", 8) == 0 && (isspace(p[8]) || p[8] == '\0' || p[8] == ';')) {
            tokens[i++].kind = TK_CONTINUE;
            p += 8;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            tokens[i++].kind = TK_FOR;
            p += 3;
            continue;
        }

        if (*p == '(') {
            tokens[i++].kind = TK_LPAREN;
            p++;
            continue;
        }

        if (*p == ')') {
            tokens[i++].kind = TK_RPAREN;
            p++;
            continue;
        }

        if (*p == '{') {
            tokens[i++].kind = TK_LBRACE;
            p++;
            continue;
        }

        if (*p == '}') {
            tokens[i++].kind = TK_RBRACE;
            p++;
            continue;
        }

        if (*p == '+') {
            p++;
            if (*p == '+') {
                tokens[i++].kind = TK_INC;
                p++;
            } else {
                tokens[i++].kind = TK_PLUS;
            }
            continue;
        }

        if (*p == '-') {
            tokens[i++].kind = TK_MINUS;
            p++;
            continue;
        }

        if (*p == '*') {
            tokens[i++].kind = TK_MUL;
            p++;
            continue;
        }

        if (*p == '/') {
            tokens[i++].kind = TK_DIV;
            p++;
            continue;
        }

        if (*p == '%') {
            tokens[i++].kind = TK_MOD;
            p++;
            continue;
        }

        if (*p == ';') {
            tokens[i++].kind = TK_SEMI;
            p++;
            continue;
        }

        if (*p == '<') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_LE;
                p++;
            } else {
                tokens[i++].kind = TK_LT;
            }
            continue;
        }

        if (*p == '>') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_GE;
                p++;
            } else {
                tokens[i++].kind = TK_GT;
            }
            continue;
        }

        if (*p == '"') {
            p++;
            int len = 0;
            while (*p != '"' && *p != '\0') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i].kind = TK_STRING;
            p++;
            i++;
            continue;
        }

        if (isalpha(*p) || *p == '_') {
            int len = 0;
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';

            if (*p == ':') {
                tokens[i].kind = TK_IDENT;
                i++;
                tokens[i++].kind = TK_COLON;
                p++;
            } else {
                tokens[i++].kind = TK_IDENT;
            }
            continue;
        }

        if (*p == '!') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_NE;
                p++;
            }
            continue;
        }

        if (*p == '=') {
            p++;
            if (*p == '=') {
                tokens[i++].kind = TK_EQ;
                p++;
            } else {
                tokens[i++].kind = TK_ASSIGN;
            }
            continue;
        }
        p++;
        printf("不明な文字ですぞ: %c\n", *p);
    }
    tokens[i].kind = TK_EOF;

    if (g_debug_token) debug_token(i);
}


int pos = 0;
Token* next_token() {
    return &tokens[pos++];
}

typedef struct {
    int breaks[128];
    int break_count;
    int continue_target;
} LoopContext;


int bytecode[1024];
LoopContext loop_stack[128];
int loop_depth = 0;
int count = 0;
void emit_op (OpCode op_code, int *val) {
    bytecode[count++] = op_code;
    if (val != NULL) {
        bytecode[count++] = *val;
    }
}

Node* parse_primary() {
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
        node = new_var_node(t->str);
    }
    return node;
}

Node* parse_term() {
    Node *node = parse_primary();

    while(tokens[pos].kind == TK_MUL || tokens[pos].kind == TK_DIV || tokens[pos].kind == TK_MOD) {
        TokenKind kind_type = tokens[pos].kind;
        next_token();
        Node *rhs = parse_primary();

        if (kind_type == TK_MUL) {
            emit_op(OP_MUL, NULL);
            node = new_binary_node(ND_MUL, node, rhs);
        } else if (kind_type == TK_DIV) {
            emit_op(OP_DIV, NULL);
            node = new_binary_node(ND_DIV, node, rhs);
        } else if (kind_type == TK_MOD) {
            emit_op(OP_MOD, NULL);
            node = new_binary_node(ND_MOD, node, rhs);
        }
    }
    return node;
}


Node* parse_expression() {
    Node *node = parse_term();
    while (tokens[pos].kind == TK_PLUS || tokens[pos].kind == TK_MINUS) {
        TokenKind kind_type = tokens[pos].kind;
        next_token();
        Node *rhs = parse_term();

        if (kind_type == TK_PLUS) {
            node = new_binary_node(ND_ADD, node, rhs);
        } else if (kind_type == TK_MINUS) {
            emit_op(OP_SUB, NULL);
            node = new_binary_node(ND_MINUS, node, rhs);
        }
    }
    return node;
}

Node* parse_evaluation() {
    Node *node = parse_expression();

    Node *rhs;
    if (tokens[pos].kind == TK_LT) {
        next_token();
        rhs = parse_expression();
        emit_op(OP_LT, NULL);
        node = new_binary_node(ND_LT, node, rhs);
    } else if (tokens[pos].kind == TK_GT) {
        next_token();
        rhs = parse_expression();
        emit_op(OP_GT, NULL);
        node = new_binary_node(ND_GT, node, rhs);
    } else if (tokens[pos].kind == TK_LE) {
        next_token();
        rhs = parse_expression();
        emit_op(OP_LE, NULL);
        node = new_binary_node(ND_LE, node, rhs);
    } else if (tokens[pos].kind == TK_GE) {
        next_token();
        rhs = parse_expression();
        emit_op(OP_GE, NULL);
        node = new_binary_node(ND_GE, node, rhs);
    } else if (tokens[pos].kind == TK_EQ) {
        next_token();
        rhs = parse_expression();
        emit_op(OP_EQ, NULL);
        node = new_binary_node(ND_EQ, node, rhs);
    } else if (tokens[pos].kind == TK_NE) {
        next_token();
        rhs = parse_expression();
        emit_op(OP_NE, NULL);
        node = new_binary_node(ND_NE, node, rhs);
    }

    return node;
}

Node* parse_statement() {
    Token *t = next_token();
    switch(t->kind) {
        case TK_PUSH: {
            Token *num = next_token();
            if (num->kind != TK_NUMBER) {
                printf("エラー: pushの次は数値を置いてくだされ");
                exit(1);
            }
            emit_op(OP_PUSH, &num->val);
            return new_num_node(&num->val);
        }
        case TK_PRINT: {
            Token *value = next_token();
            Node *var = NULL;
            if (value->kind == TK_IDENT) {
                var = new_var_node(value->str);
            } else if (value->kind == TK_NUMBER) {
                var = new_num_node(&value->val);
            }
            next_token();
            return new_unary_node(ND_PRINT, var);
        }
        case TK_IDENT: {
            if (tokens[pos].kind == TK_COLON) {
                next_token();
            }

            Node *lhs = new_var_node(t->str);
            if (tokens[pos].kind == TK_ASSIGN) {
                next_token();
                Node *rhs = parse_evaluation();
                if (tokens[pos].kind == TK_SEMI) {
                    return new_binary_node(ND_ASSIGN, lhs, rhs);
                }
            }

            if (tokens[pos].kind == TK_INC) {
                next_token();
                int addr = find_variable(t->str);
                emit_op(OP_INC, &addr);
                Node *var = new_var_node(t->str);
                return new_unary_node(ND_INC, var);
            }
            return NULL;
        }
        case TK_IF: {
            return parse_if();
        }
        case TK_WHILE: {
            return parse_while();
        }
        case TK_BREAK: {
            if (loop_depth == 0) {
                printf("エラー: ループ分の中でしか、breakは使えません。");
                exit(1);
            }
            int index = loop_stack[loop_depth].break_count++;
            loop_stack[loop_depth].breaks[index] = count;
            int zero = 0;
            emit_op(OP_JMP, &zero);
            return new_simple_node(ND_BREAK);
        }
        case TK_CONTINUE: {
            if (loop_depth == 0) {
                printf("エラー: ループ分の中でしか、continueは使えません。");
                exit(1);
            }
            int my_jmp_idx = loop_stack[loop_depth].continue_target;
            emit_op(OP_JMP, &my_jmp_idx);
            return new_simple_node(ND_CONTINUE);
        }
        case TK_FOR: {
            return parse_for();
        }
        default:
            return NULL;
    }
}

Node* parse_if () {
    if (tokens[pos].kind == TK_LPAREN) next_token();
    Node *condition = parse_evaluation();
    if (tokens[pos].kind == TK_RPAREN) next_token();

    int my_jz_idx = count;
    int zero = 0;
    emit_op(OP_JZ, &zero);

    if (tokens[pos].kind == TK_LBRACE) next_token();
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
    if (tokens[pos].kind == TK_RBRACE) next_token();

    Node *else_head = NULL;
    Node *else_tail = NULL;
    Node *else_stmt = NULL;
    if (tokens[pos].kind == TK_ELSE) {
        next_token();

        int my_jmp_idx = count;
        emit_op(OP_JMP, &zero);
        bytecode[my_jz_idx + 1] = count; 

        if (tokens[pos].kind == TK_IF) {
            next_token();
            else_head = parse_if();
            bytecode[my_jmp_idx + 1] = count;
        } else {
            if (tokens[pos].kind == TK_LBRACE) next_token();
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
            if (tokens[pos].kind == TK_RBRACE) next_token();
            bytecode[my_jmp_idx + 1] = count;
        }

    } else {
        bytecode[my_jz_idx + 1] = count;
    }
    return new_if_node(ND_IF, condition, then_head, else_head);
}

Node* parse_while() {
    loop_depth++;
    loop_stack[loop_depth].break_count = 0;
    if (tokens[pos].kind == TK_LPAREN) next_token();
    int my_jmp_idx = count;
    loop_stack[loop_depth].continue_target = my_jmp_idx;
    Node *condition = parse_evaluation();
    if (tokens[pos].kind == TK_RPAREN) next_token();
    int my_jz_idx = count;
    int zero = 0;
    emit_op(OP_JZ, &zero);

    if (tokens[pos].kind == TK_LBRACE) next_token();
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
    if (tokens[pos].kind == TK_RBRACE) next_token();

    emit_op(OP_JMP, &my_jmp_idx);
    bytecode[my_jz_idx + 1] = count;


    for (int i = 0; i < loop_stack[loop_depth].break_count; i++) {
        int break_jz_idx = loop_stack[loop_depth].breaks[i];
        bytecode[break_jz_idx + 1] = count;
    }
    loop_depth--;

    return new_loop_node(ND_WHILE, condition, body_head);
}

Node* parse_for() {
    loop_depth++;
    loop_stack[loop_depth].break_count = 0;

    if (tokens[pos].kind == TK_LPAREN) next_token();

    Node *init = NULL;
    Node *var = NULL;
    if (tokens[pos].kind == TK_IDENT) {
        Token *t = next_token();
        if (tokens[pos].kind == TK_ASSIGN) {
            next_token();
            init = parse_evaluation();
            if (tokens[pos].kind == TK_SEMI) {
                next_token();
                int addr = find_variable(t->str);
                emit_op(OP_STORE, &addr);
                var = new_var_node(t->str);
            }
        }
    }
    init = new_binary_node(ND_ASSIGN, var, init);

    int cond_start_idx = count;

    Node *condition = NULL;
    if (tokens[pos].kind != TK_SEMI) {
        condition = parse_evaluation();
    }
    if (tokens[pos].kind == TK_SEMI) next_token();

    int my_jz_idx = count;
    int zero = 0;
    emit_op(OP_JZ, &zero);

    int jump_to_body_idx = count;
    emit_op(OP_JMP, &zero);

    int update_start_idx = count;
    loop_stack[loop_depth].continue_target = update_start_idx;
    Node *update = NULL;
    if (tokens[pos].kind == TK_IDENT) {
        Token *t = next_token();
        if (tokens[pos].kind == TK_INC) {
            next_token();
            int addr = find_variable(t->str);
            emit_op(OP_INC, &addr);
            Node *var = new_var_node(t->str);
            update = new_unary_node(ND_INC, var);
        }
    }
    if (tokens[pos].kind == TK_RPAREN) next_token();
    emit_op(OP_JMP, &cond_start_idx);
    bytecode[jump_to_body_idx + 1] = count;

    if (tokens[pos].kind == TK_LBRACE) next_token();

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
    if (tokens[pos].kind == TK_RBRACE) next_token();

    emit_op(OP_JMP, &update_start_idx);
    bytecode[my_jz_idx + 1] = count;

    for (int i = 0; i < loop_stack[loop_depth].break_count; i++) {
        int break_jz_idx = loop_stack[loop_depth].breaks[i];
        bytecode[break_jz_idx + 1] = count;
    }
    loop_depth--;
    return new_for_node(ND_FOR, init, condition, update, body_head);
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
    
    if (g_debug_ast) {
        debug_ast_node(program, 1);
    }

    generate(program);

    emit_op(OP_HALT, NULL);

    if (g_debug_binary) {
        debug_bynary();
    }

    FILE *dest = fopen(argv[arg + 1], "wb");
    fwrite(bytecode, sizeof(int), count, dest);
    fclose(dest);

    printf("絶景かな！ Compiled study.goe to study.gb\n");
    return 0;
}

// =========================
// GENERATE
// =========================
void generate(Node *node) {
    if (node == NULL) return;
    while (node) {

        switch (node->kind) {
            case ND_NUM: {
                emit_op(OP_PUSH, &node->val);
                break;
            }
            case ND_ASSIGN: {
                generate(node->rhs);
                int addr = find_variable(node->lhs->name);
                emit_op(OP_STORE, &addr);
                break;
            }
            case ND_VAR: {
                int addr = find_variable(node->name);
                emit_op(OP_LOAD, &addr);
                break;
            }
            case ND_PRINT: {
                generate(node->lhs);
                emit_op(OP_PRINT, NULL);
                break;
            }
            case ND_ADD: {
                generate(node->lhs);
                generate(node->rhs);
                emit_op(OP_ADD, NULL);
                break;
            }
            default: 
                break;
        }

        node = node->next;
    }
}

// =========================
// AST
// =========================
Node* new_num_node (int *val) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_NUM;
    node_tree[current_idx].val = *val;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;

    return &node_tree[current_idx];
}

Node* new_var_node (char *str) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_VAR;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;
    strcpy(node_tree[current_idx].name, str);

    return &node_tree[current_idx];
}

Node* new_binary_node(NodeKind kind, Node* node1, Node* node2) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node1;
    node_tree[current_idx].rhs = node2;

    return &node_tree[current_idx];
}

Node* new_simple_node(NodeKind kind) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;

    return &node_tree[current_idx];
}

Node* new_unary_node(NodeKind kind, Node* node) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node;

    return &node_tree[current_idx];
}

Node* new_if_node(NodeKind kind, Node* condition, Node* then, Node* else_stmt) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].condition = condition;
    node_tree[current_idx].body = then;
    node_tree[current_idx].else_stmt = else_stmt;

    return &node_tree[current_idx];
}

Node* new_loop_node(NodeKind kind, Node* condition, Node* body) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].condition = condition;
    node_tree[current_idx].body = body;

    return &node_tree[current_idx];
}

Node* new_for_node(NodeKind kind, Node* init, Node* condition, Node* update, Node* body) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].init = init;
    node_tree[current_idx].condition = condition;
    node_tree[current_idx].update = update;
    node_tree[current_idx].body = body;

    return &node_tree[current_idx];
}

const char* node_kind_name(NodeKind kind) {
    switch(kind) {
        case ND_NUM: return "NUM";
        case ND_ADD: return "ADD";
        case ND_ASSIGN: return "ASSIGN";
        case ND_VAR: return "VAR";
        default: return "UNKNOWN";
    }
}

void print_indent(int depth) {
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
            node->kind == ND_ASSIGN ? "ASSIGN" :
            node->kind == ND_VAR ? "VAR" :
            node->kind == ND_PRINT ? "PRINT" :
            node->kind == ND_IF ? "IF" :
            node->kind == ND_INC ? "INC" :
            node->kind == ND_WHILE ? "WHILE" :
            node->kind == ND_BREAK ? "BREAK" :
            node->kind == ND_CONTINUE ? "CONTINUE" :
            node->kind == ND_FOR ? "FOR" :
            "UNKNOWN"
        );

        if (node->kind == ND_VAR) {
            printf("(%s)", node->name);
        }

        if (node->kind == ND_NUM) {
            printf(" val=%d", node->val);
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
        } else {
            debug_ast_node(node->lhs, depth + 1);
            debug_ast_node(node->rhs, depth + 1);
        }
        node = node->next;
    }
}

void print_ast() {
    printf("\n===== AST DUMP =====\n");
    for (int i = 0; i < node_depth; i++) {
        printf("%d\n", node_tree[i].val);
    }
    printf("==========================\n");
}

// =========================
// Debug Utilities
// =========================
const char *token_kind_name[] = {
    "TK_PUSH",
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
    "TK_EOF"
};
void debug_bynary() {
    printf("\n===== BINARY DUMP =====\n");
    for (int i = 0; i < count; i++) {
        printf("%d\n", bytecode[i]);
    }
    printf("==========================\n");
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
