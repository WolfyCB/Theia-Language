#ifndef THEIA_TOKENS_H
#define THEIA_TOKENS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NIL=0, INTEGER, FLOATING, STRING, CHARACTER, TRUE_BOOL, FALSE_BOOL,
    VOID=0, LET, CHAR, UCHAR, STR, BOOL, INT, UINT, FLOAT, DOUBLE, ENUM, SHORT, LONG,
    END=0, IF, ELIF, ELSE, FOR, WHILE, DO, SWITCH, RETURN, CONTINUE, BREAK,
    L_PAREN = 0, R_PAREN, L_BRACKET, R_BRACKET, L_KEY, R_KEY, DOT, COMMA, SEMICOLON, COLON, QUESTION,
    UNARY_PLUS = 0, UNARY_MINUS,
    SUM = 0, SUB, MULT, DIV, MOD,
    EQU = 0, NEQ, GRT, GRE, LST, LSE,
    EQUAL = 0, SUM_EQUAL, SUB_EQUAL, MULT_EQUAL, DIV_EQUAL, MOD_EQUAL,
    LOGIC_NOT = 0, LOGIC_AND, LOGIC_OR ,
    VAR_ID = 0, FN_ID, OBJECT_ID, MODULE_ID,
    FN = 0, OBJECT, CONSTRUCT, DESTRUCT, TEMPLATE, IMPORT, EXPORT ,
    ERR = -1,
} TokenType;
typedef enum {
    UNKNOWN = 0,
    IDENT, //1
    VALUE, DATA_TYPE, //2 3 
    LOGIC, UNARY, RATIONAL, ARITH, ASSIGN, PUNCT, //4 5 6 7 8 9
    CONTROL, KEYWORD, //10 11
    END_OF_FILE //12
} TokenClass;

typedef struct {
	const char* data;
	TokenClass Class;
	TokenType type;
} TokenElement;

typedef union {
	signed char 			c;		//char
	unsigned char 			uc;		//uchar
	signed long long int	i;		//int
	unsigned long long int 	u;		//uint
	float 					f;		//float
	double 					d;		//double
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
    Token* tokens;
    unsigned long size, cap;
} TokenVec;

extern int cmptoken(const Token, const Token);
extern TokenVec newvec();
extern void vecdel(TokenVec*);
extern void vecadd(TokenVec*, const Token);

extern TokenElement keywords[];
extern TokenElement puncts[];
extern TokenElement operators[];

#ifdef __cplusplus
}
#endif

#endif
