#ifndef MEMORY_H
#define MEMORY_H

extern int *memory;
void init_memory(void);
void memory_store(int val, int addr);
#endif