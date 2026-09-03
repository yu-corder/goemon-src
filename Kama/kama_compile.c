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
#include "opcode.h"
#include "header.h"
#include "codegen.h"

int pos = 0;

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