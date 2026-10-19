#include "stack.h"
#include <stdlib.h>

int *stack = NULL;

void init_stack(void) {
    stack = malloc(sizeof(int) * 1024);
}


