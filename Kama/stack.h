#ifndef STACK_H
#define STACK_H

extern int *stack;
void init_stack(void);
void stack_push(int val, int sp);
#endif