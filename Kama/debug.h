#ifndef DEBUG_H
#define DEBUG_H
#include "token.h"

extern const char *token_kind_name[];

const char* token_name(TokenKind kind);

void debug_token(int count);
#endif