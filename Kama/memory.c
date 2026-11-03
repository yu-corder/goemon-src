#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int *memory = NULL;
static int memory_capacity = 0;

void init_memory(void) {
    memory_capacity = 128;
    memory = calloc(memory_capacity, sizeof(int));

    if (memory == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to allocate memory\n");
        exit(1);
    }
}

static void make_bigger(void) {
    int old_capacity = memory_capacity;
    int new_capacity = memory_capacity * 2;
    int *new_stack = realloc(memory, sizeof(int) * new_capacity);

    if (new_stack == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize memory\n");
        exit(1);
    }

    memory = new_stack;
    memset(
        memory + old_capacity,
        0,
        sizeof(int) * (new_capacity - old_capacity)
    );

    memory_capacity = new_capacity;
}

void memory_store(int val, int addr) {
    if (addr < 0) {
        fprintf(stderr,
            "Runtime Error: Invalid memory address\n");
        exit(1);
    }
    
    while (addr >= memory_capacity) {
        make_bigger();
    }
    memory[addr] = val;
}