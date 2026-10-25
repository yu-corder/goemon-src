#include "frame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int frame_capacity = 0;

Frame *frame_stack = NULL;

void init_fames(void) {
    frame_capacity = 128;
    frame_stack = calloc(frame_capacity, sizeof(*frame_stack));

    for (int i = 0; i < frame_capacity; i++) {
        frame_stack[i].depths = calloc(
            128,
            sizeof(*frame_stack[i].depths)
        );

        frame_stack[i].capacity = 128;
        for (size_t j = 0; j < frame_stack[i].capacity; j++) {
            frame_stack[i].depths[j].values = calloc(
                128,
                sizeof(int)
            );

            frame_stack[i].depths[j].capacity = 128;
        }
    }
}

void free_frames(void) {
    for (int i = 0; i < frame_capacity; i++) {
        for (size_t j = 0; j < frame_stack[i].capacity; j++) {
            free(frame_stack[i].depths[j].values);
        }

        free(frame_stack[i].depths);
    }
    free(frame_stack);
    frame_stack = NULL;
}


// void make_flames_bigger(void) {
//     int old_capacity = frame_capacity;
//     int new_capacity = frame_capacity * 2;
//     int (*new_frames)[128][128] = realloc(frames, sizeof(*frames) * new_capacity);

//     if (new_frames == NULL) {
//         fprintf(stderr,
//             "Runtime Error: Failed to resize frame\n");
//         exit(1);
//     }

//     frames = new_frames;
//     memset(
//         frames + old_capacity,
//         0,
//         sizeof(*frames) * (new_capacity - old_capacity)
//     );

//     frame_capacity = new_capacity;
// }