#include "call_stack.h"
#include <stdlib.h>
#include <stdio.h>

int *call_stack = NULL;
static int call_stack_capacity = 0;

void init_call_stack(void) {
    call_stack = malloc(sizeof(int) * 128);
    call_stack_capacity = 128;
}

static void make_bigger(void) {
    int new_capacity = call_stack_capacity * 2;
    int *new_stack = realloc(call_stack, sizeof(int) * new_capacity);

    if (new_stack == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize call stack\n");
        exit(1);
    }

    call_stack_capacity = new_capacity;
    call_stack = new_stack;
}

void call_stack_push(int val, int call_sp) {
    if (call_sp == call_stack_capacity) {
        make_bigger();
    }
    call_stack[call_sp] = val;
}