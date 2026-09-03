#include "header.h"
#include "codegen.h"

GoemonHeader header() {
    GoemonHeader header;
    header.magic = 1.0;
    header.version = 1.0;
    header.bytecode_size = count;
    header.string_count = string_count;

    return header;
}