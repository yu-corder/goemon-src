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


void make_flames_bigger(void) {
    int old_capacity = frame_capacity;
    int new_capacity = frame_capacity * 2;
    Frame *new_frame_stack = realloc(frame_stack, sizeof(*frame_stack) * new_capacity);

    if (new_frame_stack == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize frame\n");
        exit(1);
    }

    frame_stack = new_frame_stack;
    for (int i = old_capacity; i < new_capacity; i++) {
        frame_stack[i].depths = calloc(
            128,
            sizeof(*frame_stack[i].depths)
        );

        if (frame_stack[i].depths == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to resize frame-depths\n");
            exit(1);
        }

        frame_stack[i].capacity = 128;
        for (size_t j = 0; j < frame_stack[i].capacity; j++) {
            frame_stack[i].depths[j].values = calloc(
                128,
                sizeof(int)
            );

            if (frame_stack[i].depths[j].values == NULL) {
                fprintf(stderr,
                    "Runtime Error: Failed to resize frame-depths-values\n");
                exit(1);
            }

            frame_stack[i].depths[j].capacity = 128;
        }
    }
    frame_capacity = new_capacity;
}

static void make_flames_depths_bigger(int call_frame) {
    int old_capacity = frame_stack[call_frame].capacity;
    int new_capacity = frame_stack[call_frame].capacity * 2;
    Depth *new_depths = realloc(frame_stack[call_frame].depths, sizeof(Depth) * new_capacity);

    if (new_depths == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize frame-depth\n");
        exit(1);
    }

    frame_stack[call_frame].depths = new_depths;

    for (int i = old_capacity; i < new_capacity; i++) {
        frame_stack[call_frame].depths[i].values = calloc(
            128,
            sizeof(int)
        );

        if (frame_stack[call_frame].depths[i].values == NULL) {
            fprintf(stderr,
                "Runtime Error: Failed to resize frame-depths-values\n");
            exit(1);
        }

        frame_stack[call_frame].depths[i].capacity = 128;
    }

    frame_stack[call_frame].capacity = new_capacity;
}

static void make_flames_values_bigger(int call_frame, int depth) {
    int old_capacity = frame_stack[call_frame].depths[depth].capacity;
    int new_capacity = frame_stack[call_frame].depths[depth].capacity * 2;
    int *new_values = realloc(frame_stack[call_frame].depths[depth].values, sizeof(int) * new_capacity);

    if (new_values == NULL) {
        fprintf(stderr,
            "Runtime Error: Failed to resize frame-depth-values\n");
        exit(1);
    }

    frame_stack[call_frame].depths[depth].values = new_values;
    memset(
        new_values + old_capacity,
        0,
        sizeof(int) * (new_capacity - old_capacity)
    );

    frame_stack[call_frame].depths[depth].capacity = new_capacity;
}


void frame_store(int call_frame, int depth, int address, int value) {
    if (call_frame < 0 || call_frame >= frame_capacity ||
        depth < 0 || address < 0) {
        fprintf(stderr, "Runtime Error: Invalid frame index\n");
        exit(1);
    }

    while ((size_t)depth >= frame_stack[call_frame].capacity) {
        make_flames_depths_bigger(call_frame);
    }

    while ((size_t)address >= frame_stack[call_frame].depths[depth].capacity) {
        make_flames_values_bigger(call_frame, depth);
    }

    frame_stack[call_frame].depths[depth].values[address] = value;
}