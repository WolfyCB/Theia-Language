#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int cmptoken(const Token a, const Token b){
    return a.Class == b.Class && a.type == b.type;
}

TokenVec newvec(){
    TokenVec vec = {
    	.data = NULL, 
    	.size = 0, 
    	.cap = 32
    };
    vec.data = (Token*)calloc(vec.cap, sizeof(Token));
    return vec;
}

void vecdel(TokenVec* vec){
    free(vec->data);
    vec->data = NULL;
    vec->cap = 0;
    vec->size = 0;
}

void vecadd(TokenVec* vec, const Token token){
    if(vec->size >= vec->cap){
        vec->cap *= 2;
        Token* tmp = (Token*)realloc(vec->data, vec->cap * sizeof(Token));
        if(!tmp){
            printf("Vector memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        vec->data = tmp;
    }
    vec->data[vec->size++] = token;
}

typedef struct {
	const char* data;
	TokenClass Class;
	TokenType type;
	unsigned int len;
} TokenElement;
typedef struct {
	TokenElement items[4];
	unsigned char count;
} TokenBucket;

static TokenBucket keywords[64];
static TokenBucket puncts[64];
static TokenBucket operators[64];

#define setToken(_name, _len, _Class, _type)\
	((TokenElement){.data=_name, .len=_len, .Class = _Class, .type = _type})
#define _HASH_ID(s, len)\
	((s[0]*0x21 + s[len/2]*0x21 + s[len-1]*0x21 + len) & 0x3f)
static inline void hashToken(TokenBucket* _tbl, const char* _name, unsigned int _len, TokenClass _class, TokenType _type){
	TokenBucket* b = &_tbl[_HASH_ID(_name, _len)];
	b->items[b->count++] = (TokenElement){.data=_name, .len=_len, .Class=_class, .type=_type};
}
	
void init_keywords(){
	hashToken(keywords, "if", 2, CONTROL, IF);
	hashToken(keywords, "do", 2, CONTROL, DO);
	hashToken(keywords, "fn", 2, KEYWORD, FN);
	hashToken(keywords, "or", 2, LOGIC, LOGIC_OR);
	
	hashToken(keywords, "int", 3, DATA_TYPE, INT);
	hashToken(keywords, "str", 3, DATA_TYPE, STR);
	hashToken(keywords, "end", 3, CONTROL, END);
	hashToken(keywords, "for", 3, CONTROL, FOR);
	hashToken(keywords, "let", 3, KEYWORD, LET);
	hashToken(keywords, "ref", 3, KEYWORD, REF);
	hashToken(keywords, "and", 3, LOGIC, LOGIC_AND);
	hashToken(keywords, "not", 3, LOGIC, LOGIC_NOT);
	
	hashToken(keywords, "true", 4, VALUE, BOOL_FALSE);
	hashToken(keywords, "char", 4, DATA_TYPE, CHAR);
	hashToken(keywords, "bool", 4, DATA_TYPE, BOOL);
	hashToken(keywords, "uint", 4, DATA_TYPE, UINT);
	hashToken(keywords, "long", 4, DATA_TYPE, LONG);
	hashToken(keywords, "size", 4, DATA_TYPE, SIZE);
	hashToken(keywords, "elif", 4, CONTROL, ELIF);
	hashToken(keywords, "else", 4, CONTROL, ELSE);
	hashToken(keywords, "case", 4, CONTROL, CASE);
	hashToken(keywords, "enum", 4, KEYWORD, ENUM);

	hashToken(keywords, "false", 5, VALUE, BOOL_TRUE);
	hashToken(keywords, "uchar", 5, DATA_TYPE, UCHAR);
	hashToken(keywords, "ulong", 5, DATA_TYPE, ULONG);
	hashToken(keywords, "short", 5, DATA_TYPE, SHORT);
	hashToken(keywords, "usize", 5, DATA_TYPE, USIZE);
	hashToken(keywords, "float", 5, DATA_TYPE, FLOAT);
	hashToken(keywords, "while", 5, CONTROL, WHILE);
	hashToken(keywords, "break", 5, CONTROL, BREAK);
	hashToken(keywords, "const", 5, KEYWORD, CONST);

	hashToken(keywords, "ushort", 6, DATA_TYPE, USHORT);
	hashToken(keywords, "double", 6, DATA_TYPE, DOUBLE);
	hashToken(keywords, "switch", 6, CONTROL, SWITCH);
	hashToken(keywords, "return", 6, CONTROL, RETURN);
	hashToken(keywords, "object", 6, KEYWORD, OBJECT);
	hashToken(keywords, "import", 6, KEYWORD, IMPORT);
	hashToken(keywords, "export", 6, KEYWORD, EXPORT);

	hashToken(keywords, "default", 7, CONTROL, DEFAULT);

	hashToken(keywords, "nextcase", 8, CONTROL, NEXTCASE);
	hashToken(keywords, "continue", 8, CONTROL, CONTINUE);
	hashToken(keywords, "destruct", 8, KEYWORD, DESTRUCT);
	hashToken(keywords, "template", 8, KEYWORD, TEMPLATE);

	hashToken(keywords, "construct", 9, KEYWORD, CONSTRUCT);
};

void init_puncts(){
	hashToken(puncts, "(", 1, PUNCT, L_PAREN);
	hashToken(puncts, ")", 1, PUNCT, R_PAREN);
	hashToken(puncts, "[", 1, PUNCT, L_BRACKET);
	hashToken(puncts, "]", 1, PUNCT, R_BRACKET);
	hashToken(puncts, "{", 1, PUNCT, L_KEY);
	hashToken(puncts, "}", 1, PUNCT, R_KEY);
	hashToken(puncts, ".", 1, PUNCT, DOT);
	hashToken(puncts, ",", 1, PUNCT, COMMA);
	hashToken(puncts, ";", 1, PUNCT, SEMICOLON);
	hashToken(puncts, ":", 1, PUNCT, COLON);
	hashToken(puncts, "$", 1, PUNCT, DOLLAR);
	hashToken(puncts, "?", 1, PUNCT, QUESTION);
	hashToken(puncts, "_", 1, PUNCT, UNDERLINE);

	hashToken(puncts, "..", 2, RANGE, RANGE_EXC);
	hashToken(puncts, "..=", 3, RANGE, RANGE_INC);
}

void init_operators(){
	hashToken(operators, "+", 1, ARITH, SUM);
	hashToken(operators, "-", 1, ARITH, SUB);
	hashToken(operators, "*", 1, ARITH, MULT);
	hashToken(operators, "/", 1, ARITH, DIV);
	hashToken(operators, "%", 1, ARITH, MOD);
	hashToken(operators, "=", 1, ASSIGN, EQUAL);
	hashToken(operators, ">", 1, RATIONAL, GRT);
	hashToken(operators, "<", 1, RATIONAL, LST);

	hashToken(operators, "+=", 2, ASSIGN, SUM_EQUAL);
	hashToken(operators, "-=", 2, ASSIGN, SUB_EQUAL);
	hashToken(operators, "*=", 2, ASSIGN, MULT_EQUAL);
	hashToken(operators, "/=", 2, ASSIGN, DIV_EQUAL);
	hashToken(operators, "%=", 2, ASSIGN, MOD_EQUAL);
	hashToken(operators, "++", 2, UNARY, UNARY_PLUS);
	hashToken(operators, "--", 2, UNARY, UNARY_MINUS);
	hashToken(operators, "==", 2, RATIONAL, EQU);
	hashToken(operators, "!=", 2, RATIONAL, NEQ);
	hashToken(operators, "<=", 2, RATIONAL, LSE);
	hashToken(operators, ">=", 2, RATIONAL, GRE);
		
}

void initTokens(){
	init_keywords();
	printf("\n");
	init_puncts();
	printf("\n");
	init_operators();
	printf("\n");
}

static inline TokenBucket* hashId(TokenBucket* tbl, const char* s, const size_t len){
    return &tbl[_HASH_ID(s, len)];
}

Token getKeywordToken(const char* str, unsigned long len){
	TokenBucket* b = hashId(keywords, str, len);
	for(int i=0; i<b->count; i++){
		TokenElement* k = &b->items[i];
		if(k->len != len) continue;
		if(k->data && 
			memcmp(k->data,str,len)==0){
			return (Token){.Class=k->Class, .type=k->type};
		}
	}
	return (Token){.Class=IDENT, .type=ID};
}
Token getPunctToken(const char* str, unsigned long len){
	TokenBucket* b = hashId(puncts, str, len);
	for(int i=0; i<b->count; i++){
		TokenElement* k = &b->items[i];
		if(k->len != len) continue;
		if(k->data && 
			memcmp(k->data,str,1)==0){
			return (Token){.Class=k->Class, .type=k->type};
		}
	}
	return (Token){.Class=UNKNOWN, .type=ERR};
}
Token getOperatorToken(const char* str, unsigned long len){
	TokenBucket* b = hashId(operators, str, len);
	for(int i=0; i<b->count; i++){
		TokenElement* k = &b->items[i];
		if(k->len != len) continue;
		if(k->data && 
			memcmp(k->data,str,len)==0){
			return (Token){.Class=k->Class, .type=k->type};
		}
	}
	return (Token){.Class=UNKNOWN, .type=ERR};
}
