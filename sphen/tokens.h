#ifndef SPHEN_TOKENS_H
#define SPHEN_TOKENS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ERR = -1,
    NIL=0, INTEGER, FLOATING, STRING, CHARACTER, BOOL_TRUE, BOOL_FALSE,
    VOID, CHAR, UCHAR, STR, BOOL, INT, UINT, FLOAT, DOUBLE, SHORT, USHORT, LONG, ULONG, SIZE, USIZE,
    END, IF, ELIF, ELSE, DO, FOR, WHILE, SWITCH, RETURN, CONTINUE, BREAK, NEXTCASE, CASE, DEFAULT,
    L_PAREN, R_PAREN, L_BRACKET, R_BRACKET, L_KEY, R_KEY, DOT, COMMA, SEMICOLON, COLON, QUESTION, DOLLAR, UNDERLINE,
    RANGE_EXC, RANGE_INC,
    UNARY_PLUS, UNARY_MINUS,
    SUM, SUB, MULT, DIV, MOD,
    EQU, NEQ, GRT, GRE, LST, LSE,
    EQUAL, SUM_EQUAL, SUB_EQUAL, MULT_EQUAL, DIV_EQUAL, MOD_EQUAL,
    LOGIC_NOT, LOGIC_AND, LOGIC_OR ,
    ID,
    FN, OBJECT, CONSTRUCT, DESTRUCT, TEMPLATE, IMPORT, EXPORT, LET, CONST, REF, ENUM,
} TokenType;
typedef enum {
    UNKNOWN = 0,
    IDENT, //1
    VALUE, DATA_TYPE, //2 3 
    RANGE, //4
    LOGIC, UNARY, RATIONAL, ARITH, ASSIGN, PUNCT, //5 6 7 8 9 10
    CONTROL, KEYWORD, //11 12
    END_OF_FILE //13
} TokenClass;


typedef union {
	signed char 			c;		//char
	unsigned char 			uc;		//uchar
	signed long long int	i;		//int
	unsigned long long int 	u;		//uint
	double 					f;		//double
	char* 					str;	//string
	int						b;		//boolean
} TokenValue;

typedef struct {
	TokenValue 	data;
	TokenClass 	Class;
    TokenType 	type;
    unsigned int col, line;
} Token;

typedef struct {
    Token* data;
    unsigned long size, cap;
} TokenVec;

extern int cmptoken(const Token, const Token);
extern TokenVec newvec();
extern void vecdel(TokenVec*);
extern void vecadd(TokenVec*, const Token);

extern void initTokens();
extern Token getKeywordToken(const char* str, unsigned long len);
extern Token getPunctToken(const char* str, unsigned long len);
extern Token getOperatorToken(const char* str, unsigned long len);

#ifdef __cplusplus
}
#endif

#endif
