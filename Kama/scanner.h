#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

void tokenize(char *src);
void free_all_tokens(void);

#endif