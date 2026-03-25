#ifndef SPHEN_LEXER_H
#define SPHEN_LEXER_H

#include <stdio.h>
#include "tokens.h"
#include "str.h"

typedef struct {
    size_t cur;
    unsigned long line, col;
    String buffer;
    unsigned long codeLen;
    char* code;
    TokenVec vec;
} Lexer;

extern Lexer lexer_init(const char*);
extern void lexer_close(Lexer*);

extern void tokenize(Lexer*);

#endif
