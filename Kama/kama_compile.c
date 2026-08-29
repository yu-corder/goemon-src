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

typedef enum {
    ND_NUM,
    ND_STR,
    ND_BOOL,
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
    ND_VAR_DECL,
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
    ND_FUNCTION,
    ND_CALL,
    ND_RET,
} NodeKind;

typedef enum {
    TY_VOID,
    TY_INT,
    TY_STRING,
    TY_BOOL,
} TypeKind;

typedef struct {
    TokenKind kind;
    int val;
    char str[256];
    bool bool_val;
    int length;
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

    struct Node *params;
    

    struct Node *next;

    int val;
    bool bool_val;
    char str[128];
    int len;
    char name[32];
    char func_name[64];

    TypeKind type;

    int address;
    int depth;
    bool is_global;
} Node;
int node_depth = 0;


typedef struct {
    char name[32];
    int address;
} Label;

typedef struct {
    char name[32];
    int memory_index;

    TypeKind type;
} Variable;

typedef struct {
    char name[64][64];
    int address[64];
    int function_count;
    TypeKind type[64];
} Funcion;

Funcion function_table[128];

typedef struct {
    char name[64][64];

    char params[16][16][32];
    int param_count[64];
    int function_count;

    TypeKind type[16][16];
} FuncionParams;

FuncionParams function_params_table[128];

typedef struct {
    int address;
    int depth;
    int param_count;

    char (*params)[32];

    TypeKind type;
    bool found;
} FuncionInfo;

typedef struct {
    int address;
    int depth;
    int param_count;

    char (*params)[32];

    bool found;

    TypeKind *type;
} FuncionParamsInfo;


Variable global_variable_table[128];
int global_variable_count = 0;

typedef struct {
    int variable_count;
    char name[32][32];
    int address[32];

    TypeKind type[32];
} LocalVariables;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} LocalVariablesInfo;

typedef struct {
    int address;
    int depth;

    bool found;

    TypeKind type;
} GlobalVariablesInfo;

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

LocalVariables local_scopes[128];
int block_depth = 0;

GoemonHeader header();

Node node_tree[128];
Node* new_num_node();
Node* new_str_node();
Node* new_bool_node();
Node* new_binary_node();
Node* new_decl_no_assignment_node();
Node* new_decl_node();
Node* new_var_node ();
Node* new_simple_node();
Node* new_unary_node();
Node* new_if_node();
Node* new_loop_node();
Node* new_for_node();
Node* new_func_node();
Node* new_call_node();
void debug_ast_node();
void print_ast();
void generate(Node* node);

void name_resolution();

TypeKind type_check();
void type_check_program();

const char* type_name(TypeKind type);
const char* token_name(TokenKind kind);

Node* parse_statement();
Node* parse_evaluation();
Node* parse_expression();
Node* parse_if();
Node* parse_while();
Node* parse_for();
Node* parse_function();
Node* parse_print();

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

GlobalVariablesInfo find_global_variable(char *name) {
    GlobalVariablesInfo var;
    var.found = false;
    var.address = -1;
    for (int i = 0; i < global_variable_count; i++) {
        if (strcmp(global_variable_table[i].name, name) == 0) {
            var.address = global_variable_table[i].memory_index;
            var.type = global_variable_table[i].type;
            var.found = true;
            return var;
        }
    }
    
    return var;
}

LocalVariablesInfo find_local_variable(char *name, int depth) {
    LocalVariablesInfo var;
    var.found = false;
    var.address = -1;
    for (int i = depth; i >= 0; i--) {
        for (int j = 0; j < local_scopes[i].variable_count; j++) {
            if (strcmp(local_scopes[i].name[j], name) == 0) {
                var.address = local_scopes[i].address[j];
                var.depth = i;
                var.found = true;
                var.type = local_scopes[i].type[j];
                return var;
            }
        }
    }
    return var;
}

int insert_global_variable(char *name, TypeKind* type) {
    int current_idx = global_variable_count;
    global_variable_count++;

    strcpy(global_variable_table[current_idx].name, name);
    global_variable_table[current_idx].type = *type;
    
    if (strncmp(name, "__s", 3) == 0) {
        global_variable_table[current_idx].memory_index = 1000 + (global_variable_count * 100);
    } else {
        global_variable_table[current_idx].memory_index = global_variable_count;
    }
    return global_variable_table[current_idx].memory_index;
}

int insert_local_variable(char *name, int depth, TypeKind* type) {
    int current_idx = local_scopes[depth].variable_count;
    local_scopes[depth].variable_count++;
    strcpy(local_scopes[depth].name[current_idx], name);
    local_scopes[depth].type[current_idx] = *type;
    
    if (strncmp(name, "__s", 3) == 0) {
        local_scopes[depth].address[current_idx] = 1000 + (current_idx * 100);;
    } else {
        local_scopes[depth].address[current_idx] = current_idx;
    }
    return local_scopes[depth].address[current_idx];
}

FuncionParamsInfo find_function_params(char *name, int depth) {
    FuncionParamsInfo var;
    var.found = false;
    var.address = -1;

    for (int i = depth + 1; i >= 0; i--) {
        for (int j = 0; j < function_params_table[i].function_count; j++) {
            if (strcmp(function_params_table[i].name[j], name) == 0) {
                var.found = true;
                var.depth = i;
                var.param_count = function_params_table[i].param_count[j];
                var.params = function_params_table[i].params[j];
                var.type = function_params_table[i].type[j];
                return var;
            }
        }
    }

    return var;
}

void insert_function_params(char *name, Node *params, int depth) {
    int current_idx = function_params_table[depth].function_count;
    

    for (int i = 0; i < function_params_table[depth].function_count; i++) {
        if (strcmp(function_params_table[depth].name[i], name) == 0) {
            return;
        }
    }

    strcpy(function_params_table[depth].name[current_idx], name);

    Node *p = params;

    int p_count = 0;
    while (p) {
        if (p->lhs == NULL) {
            fprintf(stderr, "Parameter '%s' requires an explicit type declaration.\n", p->name);
            exit(1);
        }
        strcpy(function_params_table[depth].params[current_idx][p_count], p->lhs->name);
        function_params_table[depth].type[current_idx][p_count] = p->type;
        p_count++;
        p = p->next;
    }
    
    function_params_table[depth].param_count[current_idx] = p_count;
    function_params_table[depth].function_count++;
}


FuncionInfo find_function(char *name, int depth) {
    FuncionInfo var;
    var.found = false;
    var.address = -1;

    for (int i = depth + 1; i >= 0; i--) {
        for (int j = 0; j < function_table[i].function_count; j++) {
            if (strcmp(function_table[i].name[j], name) == 0) {
                var.found = true;
                var.address = function_table[i].address[j];
                var.depth = i;
                var.type = function_table[i].type[j];
                return var;
            }
        }
    }

    return var;
}

void insert_function(char *name, int address, int depth, TypeKind type) {
    int current_idx = function_table[depth].function_count;
    

    for (int i = 0; i < function_table[depth].function_count; i++) {
        if (strcmp(function_table[depth].name[i], name) == 0) {
            return;
        }
    }

    strcpy(function_table[depth].name[current_idx], name);
    function_table[depth].address[current_idx] = address;
    function_table[depth].function_count++;
    function_table[depth].type[current_idx] = type;
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

Token tokens[MAX_TOKENS];
int line = 1;
void tokenize (char *p) {
    int i = 0;
    while(*p) {
        if (*p == '\n') {
            p++;
            line++;
            continue;
        }

        if (isspace(*p)) { p++; continue;}

        if (isdigit(*p)) {
            tokens[i].kind = TK_NUMBER;
            tokens[i].val = strtol(p, &p, 10);
            i++;
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

        if (strncmp(p, "function", 8) == 0 && (isspace(p[8]) || p[8] == '\0')) {
            tokens[i++].kind = TK_FUNCTION;
            p += 8;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && (isspace(p[6]) || p[6] == '\0')) {
            tokens[i++].kind = TK_RET;
            p += 6;
            continue;
        }

        if (strncmp(p, "int", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            p += 3;
            int len = 0;
            tokens[i++].kind = TK_INT;

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i++].kind = TK_IDENT;
            continue;
        }

        if (strncmp(p, "str", 3) == 0 && (isspace(p[3]) || p[3] == '\0')) {
            p += 3;
            int len = 0;
            tokens[i++].kind = TK_STRING_TYPE;

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i++].kind = TK_IDENT;
            continue;
        }

        if (strncmp(p, "bool", 4) == 0 && (isspace(p[4]) || p[4] == '\0')) {
            p += 4;
            int len = 0;
            tokens[i++].kind = TK_BOOL_TYPE;

            while (isspace(*p)) {
                p++;
            }
            
            while (isalnum(*p) || *p == '_') {
                tokens[i].str[len++] = *p++;
            }
            tokens[i].str[len] = '\0';
            tokens[i++].kind = TK_IDENT;
            continue;
        }

        if (strncmp(p, "true", 4) == 0 && (isspace(p[4]) || p[4] == '\0' || p[4] == ';' || p[4] == ')')) {
            tokens[i].kind = TK_BOOL;
            tokens[i].bool_val = true;
            i++;
            p += 4;
            continue;
        }

        if (strncmp(p, "false", 5) == 0 && (isspace(p[5]) || p[5] == '\0' || p[5] == ';' || p[5] == ')')) {
            tokens[i].kind = TK_BOOL;
            tokens[i].bool_val = false;
            i++;
            p += 5;
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
            tokens[i].length = len;
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

        if (*p == ',') {
            p++;
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

        printf("Line %d: Unknown character '%c'\n", line, *p);
        exit(1);
    }
    tokens[i].kind = TK_EOF;

    if (g_debug_token) debug_token(i);
}


int pos = 0;
Token* next_token() {
    return &tokens[pos++];
}

Token* prev_token() {
    return &tokens[pos--];
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


void emit_count_reset() {
    count = 0;
}

void emit_count_up() {
    count++;
}

void emit_count_two_up() {
    count++;
    count++;
}

void emit_count_three() {
    count++;
    count++;
    count++;
}

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

bool consume(TokenKind kind) {
    if (tokens[pos].kind != kind) {
        return false;
    }

    next_token();
    return true;
}

TypeKind token_convert_type(TokenKind kind) {
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

TypeKind ret_type() {
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

Token *expect_ident() {
    if (tokens[pos].kind != TK_IDENT) {
        fprintf(stderr,
            "Expected TK_IDENT but got %s\n",
            token_name(tokens[pos].kind));
        exit(1);
    }

    Token *tok = next_token();
    return tok;
}

Token *expect_bool() {
    if (tokens[pos].kind != TK_BOOL) {
        fprintf(stderr,
            "Expected TK_BOOL but got %s\n",
            token_name(tokens[pos].kind));
        exit(1);
    }

    Token *tok = next_token();
    return tok;
}

void expect(TokenKind kind) {
    if (!consume(kind)) {
        fprintf(stderr,
            "Expected %s but got %s\n",
            token_name(kind),
            token_name(tokens[pos].kind));
        exit(1);
    }
}

Node* parse_statement_list(TokenKind kind) {
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

Node* parse_argument_list(TokenKind kind) {
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

Node* parse_term() {
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


Node* parse_expression() {
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

Node* parse_binary(Node *lhs, NodeKind kind) {
    next_token();
    Node *rhs = parse_expression();
    return new_binary_node(kind, lhs, rhs);
}

Node* parse_evaluation() {
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

Node* parse_statement() {
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

Node* parse_function() {
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

Node* parse_if () {
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

Node* parse_while() {
    consume(TK_LPAREN);

    Node *condition = parse_evaluation();

    consume(TK_RPAREN);

    consume(TK_LBRACE);
    Node *body_head = parse_statement_list(TK_RBRACE);

    consume(TK_RBRACE);

    return new_loop_node(ND_WHILE, condition, body_head);
}

Node* parse_for() {
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
// NAME RESOLUTION
// =========================

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

                int func_start_address = count;
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

int local_scope = 0;

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
                }

                if (node->lhs->is_global) {
                    emit_one_operand(OP_STORE, &node->lhs->address);
                } else {
                    emit_two_operand(OP_STORE_LOCAL, &node->lhs->address, &node->lhs->depth);
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

Node* new_bool_node (bool *val) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_BOOL;
    node_tree[current_idx].bool_val = val;
    node_tree[current_idx].lhs = NULL;
    node_tree[current_idx].rhs = NULL;

    node_tree[current_idx].val = val ? 1 : 0;

    return &node_tree[current_idx];
}

Node* new_str_node (char *str, int *len) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = ND_STR;
    strcpy(node_tree[current_idx].str, str);
    node_tree[current_idx].len = *len;
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

Node* new_decl_no_assignment_node(NodeKind kind, Node* node1, TypeKind type) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node1;
    node_tree[current_idx].type = type;

    return &node_tree[current_idx];
}

Node* new_decl_node(NodeKind kind, Node* node1, Node* node2, TypeKind type) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    node_tree[current_idx].lhs = node1;
    node_tree[current_idx].rhs = node2;
    node_tree[current_idx].type = type;

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

Node* new_func_node(NodeKind kind, char *str, Node* params, Node* body, TypeKind type) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    strcpy(node_tree[current_idx].func_name, str);
    
    node_tree[current_idx].params = params;
    node_tree[current_idx].body = body;
    node_tree[current_idx].type = type;

    return &node_tree[current_idx];
}

Node* new_call_node(NodeKind kind, char *str, Node* params) {
    int current_idx = node_depth;
    node_depth++;

    node_tree[current_idx].kind = kind;
    strcpy(node_tree[current_idx].func_name, str);

    node_tree[current_idx].params = params;

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

const char* type_name(TypeKind kind) {
    switch(kind) {
        case TY_INT: return "INT";
        case TY_STRING: return "STRING";
        case TY_BOOL: return "BOOL";
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
    "TK_EOF"
};

const char* token_name(TokenKind kind) {
    return token_kind_name[kind];
}
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
