#ifndef FRAME_H
#define FRAME_H

extern int (*frames)[128][128];
extern int frame_capacity;

void init_fames(void);
void make_flames_bigger(void);
#endif