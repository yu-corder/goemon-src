#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

int *stack = NULL;
static int stack_capacity = 0;

void init_stack(void) {
    stack = malloc(sizeof(int) * 1024);
    stack_capacity = 1024;
}

static void make_bigger(void) {
    int new_capacity = stack_capacity * 2;
    int *new_stack = realloc(stack, sizeof(int) * new_capacity);

    if (new_stack == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize stack\n");
        exit(1);
    }

    stack_capacity = new_capacity;
    stack = new_stack;
}

void stack_push(int val, int sp) {
    if (sp == stack_capacity) {
        make_bigger();
    }
    stack[sp] = val;
}



