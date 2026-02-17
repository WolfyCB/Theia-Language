#ifndef FASM_LEXER_H
#define FASM_LEXER_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "tokens.h"
#include "tools.h"

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
extern int advance(Lexer*);
extern int peek(Lexer*);
extern int next(Lexer*);
extern void consume(Lexer*);
extern void fabricToken(Lexer*, const Token);

extern void tokenize(Lexer*);

#ifdef __cplusplus
}
#endif
#endif
