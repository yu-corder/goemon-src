#include "frame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int (*frames)[128][128] = NULL;
static int frame_capacity = 0;

void init_fames(void) {
    frames = calloc(128, sizeof(*frames));

    if (frames == NULL) {
        fprintf(stderr, "Runtime Error: Failed to allocate frames\n");
        exit(1);
    }
    frame_capacity = 128;
}