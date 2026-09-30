#ifndef KAMA_STRING_H
#define KAMA_STRING_H

typedef struct {
    char str[32];
    int length;
} String;

extern String *string_table;
extern int string_count;
extern int string_capacity;
void init_strings(void);
void make_bigger(void);

#endif