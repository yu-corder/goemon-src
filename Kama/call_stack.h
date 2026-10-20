#ifndef CALL_STACK_H
#define CALL_STACK_H

extern int *call_stack;
void init_call_stack(void);
void call_stack_push(int val, int call_sp);
#endif