#ifndef MEMORY_H
#define MEMORY_H

extern int *memory;
void init_memory(void);
void memory_store(int val, int addr);
int memory_load(int addr);
void memory_incrment(int addr);
void free_memory(void);
#endif