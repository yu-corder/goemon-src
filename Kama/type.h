#ifndef TYPE_H
#define TYPE_H

typedef enum {
    TY_VOID,
    TY_INT,
    TY_STRING,
    TY_BOOL,
} TypeKind;

typedef struct Node Node;

void type_check_program(Node *program);

#endif