#ifndef FRAME_H
#define FRAME_H
#include <stddef.h>

typedef struct {
    int *values;
    size_t capacity;
} Depth;

typedef struct {
    Depth *depths;
    size_t capacity;
} Frame;

extern Frame *frame_stack;
extern int frame_capacity;

void init_fames(void);
void free_frames(void);
void make_flames_bigger(void);

void frame_store(int call_frame, int depth, int address, int value);
#endif