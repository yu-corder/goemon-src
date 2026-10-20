#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

int *memory = NULL;
static int memory_capacity = 0;

void init_memory(void) {
    memory = calloc(128, sizeof(int));
    memory_capacity = 128;
}

static void make_bigger(void) {
    int new_capacity = memory_capacity * 2;
    int *new_stack = realloc(memory, sizeof(int) * new_capacity);

    if (new_stack == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize memory\n");
        exit(1);
    }

    memory_capacity = new_capacity;
    memory = new_stack;
}

void memory_store(int val, int addr) {
    if (addr == memory_capacity) {
        make_bigger();
    }
    memory[addr] = val;
}