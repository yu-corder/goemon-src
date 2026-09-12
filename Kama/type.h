#ifndef TYPE_H
#define TYPE_H

typedef enum {
    TY_VOID,
    TY_INT,
    TY_STRING,
    TY_BOOL,
    TY_INT_ARRAY,
} TypeKind;

typedef struct Node Node;

void type_check_program(Node *program);

#endif