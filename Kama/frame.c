#include "frame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int (*frames)[128][128] = NULL;
int frame_capacity = 0;

void init_fames(void) {
    frames = calloc(128, sizeof(*frames));

    if (frames == NULL) {
        fprintf(stderr, "Runtime Error: Failed to allocate frames\n");
        exit(1);
    }
    frame_capacity = 128;
}

void make_flames_bigger(void) {
    int old_capacity = frame_capacity;
    int new_capacity = frame_capacity * 2;
    int (*new_frames)[128][128] = realloc(frames, sizeof(*frames) * new_capacity);

    if (new_frames == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize frame\n");
        exit(1);
    }

    frames = new_frames;
    memset(
        frames + old_capacity,
        0,
        sizeof(*frames) * (new_capacity - old_capacity)
    );

    frame_capacity = new_capacity;
}