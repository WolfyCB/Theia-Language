#ifndef SPHEN_PARSER_H
#define SPHEN_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexer.h"
#include "ast.h"

typedef struct {
	const char* message;
	unsigned long line, col;
} DebugMessage;

typedef struct {
	DebugMessage* data;
	unsigned long len, cap;
} ParserDebug;


typedef struct {
	ASTVec nodes;
	Lexer lexer;
	ParserDebug errors;
	ParserDebug warns;
	unsigned long long int id, pos, count;
} Parser;


extern Parser parser_init(const char*);
extern void parser_close(Parser*);
extern int parse(Parser*);

#ifdef __cplusplus
}
#endif

#endif
