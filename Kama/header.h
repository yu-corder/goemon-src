#ifndef HEADER_H
#define HEADER_H
#include <stdint.h>

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t bytecode_size;
    uint32_t string_count;
} GoemonHeader;

GoemonHeader header();

#endif