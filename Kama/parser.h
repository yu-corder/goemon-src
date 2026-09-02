#ifndef PARSER_H
#define PARSER_H
#include <stdbool.h>
#include "ast.h"
#include "type.h"
#include "token.h"
#include "debug.h"

Node* parse_program();
#endif