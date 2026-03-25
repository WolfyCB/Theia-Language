#ifndef SPHEN_AST_H
#define SPHEN_AST_H

#include "tokens.h"

typedef enum {
	AST_UNKNOWN = 0,
	AST_BLOCK,
	AST_FILE,

    AST_VAR_DECL,
    AST_FN_DECL,
    AST_FN_PARAM,
	AST_OBJ_DECL,
	AST_OBJ_CONSTRUCT_DECL,
	AST_OBJ_DESTRUCT_DECL,
	
    AST_VAR_CALL,
    AST_FN_CALL,

    AST_IF_STMT,
    AST_ELIF_STMT,
    AST_ELSE_STMT,
    AST_FOR_STMT,
    AST_WHILE_STMT,
    AST_DOWHILE_STMT,
    AST_SWITCH_STMT,
    AST_CASE_STMT,
    AST_DEFAULT_STMT,
    AST_RETURN_STMT,
    AST_BREAK_STMT,
    AST_CONTINUE_STMT,
    AST_IMPORT_STMT,
    AST_EXPORT_STMT,

    AST_LITERAL,
    AST_EXPR,
    AST_TYPE,
    AST_TUPLE_EXPR,
    AST_TUPLE_TYPE,
    AST_UNARY,
    AST_RETURN
} ASTType;
typedef enum {
    PREC_NONE = 0,
    PREC_ASSIGN,     // = += -= /= *=
    PREC_OR,         // ||
    PREC_AND,        // &&
    PREC_EQUALITY,   // == !=
    PREC_COMPARE,    // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * / %
    PREC_UNARY,      // ! -
    PREC_PRIMARY
} ASTPrecedence;
typedef enum {
	ATTR_LET = 0,
	ATTR_CONST,
	ATTR_REF
} ATTRtype;


typedef unsigned long int NodeId;
typedef struct VecId VecId;
typedef struct ASTVec ASTVec;
typedef struct ASTNode ASTNode;


typedef struct {
	unsigned long begin;
	unsigned long count;
	NodeId last;
} ASTblock;
typedef struct {
	unsigned long begin;
	unsigned long count;
} ASTgap;
typedef struct {
	char* path;
} ASTfile;
typedef struct {
	NodeId left;
	TokenType op;
	NodeId right;
	unsigned long int size;
} ASTexpr;
typedef struct {
	ASTgap values;
	unsigned int count;
} ASTtuple;
typedef struct {
	TokenValue data;
	TokenType type;
} ASTliteral;
typedef struct {
	union {
		NodeId typeId;
		TokenType typeP;
	};
	int isId;
} ASTdata_type;
typedef struct {
	ATTRtype attr;
	NodeId type;
	char* name;
	NodeId value;
} ASTvar;
typedef struct {
	ATTRtype attr;
	NodeId type;
	char* name;
	NodeId std_value;
} ASTparam;
typedef struct {
	char* name;
	ASTgap param;
	NodeId ret_type;
	ASTblock block;
} ASTfunc;
typedef struct {
	ASTblock block;
	NodeId cond;
} ASTwhile;
typedef struct {
	ASTblock block;
	NodeId cond;
} ASTdo_while;
typedef struct {
	ASTblock then_block;
	NodeId else_block;
	NodeId cond;
} ASTif;
typedef struct {
	ASTblock block;
} ASTelse;

typedef struct {
	NodeId cond;
	ASTblock body;
} ASTcase;
typedef struct {
	NodeId value;
	ASTgap cases;
	NodeId default_case;
} ASTswitch;

typedef struct {
	NodeId expr;
} ASTreturn;



typedef struct ASTNode {
    union {
		ASTfile file;
		ASTexpr expr;
		ASTliteral literal;
		ASTdata_type data_type;
		ASTvar var;
		ASTfunc func;
		ASTparam param;
		ASTwhile while_st;
		ASTdo_while dowhile_st;
		ASTif if_st;
		ASTelse else_st;
		ASTswitch switch_st;
		ASTcase case_st;
		ASTreturn return_st;
		ASTtuple tuple;
    };
    NodeId id;
    ASTType type;
} ASTNode;


typedef struct VecId {
	NodeId* data;
	unsigned long int len, cap;
} VecId;
typedef struct ASTVec {
	ASTNode* data;
	unsigned long int len, cap;
} ASTVec;

extern int newastvec(ASTVec*);
extern ASTNode getnode(ASTVec*, NodeId);
extern int pushnode(ASTVec*, const ASTNode);
extern int delast(ASTVec*);

extern int newvecid(VecId*, unsigned long);
extern NodeId getvecid(VecId*, NodeId);
extern int pushvecid(VecId*, NodeId);
extern int delvecid(VecId*);

#endif
