#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>


int cmptoken(const Token a, const Token b){
    return a.token == b.token && a.subToken == b.subToken;
}

TokenVec newvec(){
    TokenVec vec = {
    	.tokens = NULL, 
    	.size = 0, 
    	.cap = 32
    };
    vec.tokens = (Token*)calloc(vec.cap, sizeof(Token));
    return vec;
}

void vecdel(TokenVec* vec){
    free(vec->tokens);
    vec->tokens = NULL;
    vec->cap = 0;
    vec->size = 0;
}

void vecadd(TokenVec* vec, const Token token){
    if(vec->size >= vec->cap){
        vec->cap *= 2;
        Token* tmp = (Token*)realloc(vec->tokens, vec->cap * sizeof(Token));
        if(!tmp){
            printf("Vector memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        vec->tokens = tmp;
    }
    vec->tokens[vec->size++] = token;
}



Token keywords[] = {
    {.data = "true",	.token = VALUE,         .subToken = TRUE_BOOL	},
    {.data = "false",	.token = VALUE,         .subToken = FALSE_BOOL	},

    //data types
    {.data = "void",        .token = DATA_TYPE,	.subToken = VOID		},
    {.data = "let",         .token = DATA_TYPE,	.subToken = LET			},
    {.data = "char",        .token = DATA_TYPE,	.subToken = CHAR		},
    {.data = "uchar",       .token = DATA_TYPE,	.subToken = UCHAR		},
    {.data = "str",         .token = DATA_TYPE,	.subToken = STR			},
    {.data = "bool",        .token = DATA_TYPE,	.subToken = BOOL		},
    {.data = "int",         .token = DATA_TYPE,	.subToken = INT			},
    {.data = "uint",        .token = DATA_TYPE,	.subToken = UINT		},
    {.data = "float",       .token = DATA_TYPE,	.subToken = FLOAT		},
    {.data = "double",      .token = DATA_TYPE,	.subToken = DOUBLE		},
    {.data = "enum",        .token = DATA_TYPE,	.subToken = ENUM		},
    
    {.data = "short",       .token = DATA_TYPE,	.subToken = SHORT		},
    {.data = "long",        .token = DATA_TYPE,	.subToken = LONG		},
    //control keywords
    {.data = "end",         .token = CONTROL,   .subToken = END 		},
    {.data = "if",          .token = CONTROL,   .subToken = IF 			},
    {.data = "elif",        .token = CONTROL,   .subToken = ELIF 		},
    {.data = "else",        .token = CONTROL,   .subToken = ELSE 		},
    {.data = "for",         .token = CONTROL,   .subToken = FOR 		},
    {.data = "while",       .token = CONTROL,   .subToken = WHILE 		},
    {.data = "do",       	.token = CONTROL,   .subToken = DO 		},
    {.data = "switch",      .token = CONTROL,   .subToken = SWITCH 		},
    {.data = "return",      .token = CONTROL,   .subToken = RETURN 		},
    {.data = "continue",    .token = CONTROL,   .subToken = CONTINUE 	        },
    {.data = "break",       .token = CONTROL,   .subToken = BREAK 		},

    {.data = "fn",          .token = KEYWORD,	.subToken = FN 			},
    {.data = "object",      .token = KEYWORD, 	.subToken = OBJECT 		},
    {.data = "construct",   .token = KEYWORD,   .subToken = CONSTRUCT 	        },
    {.data = "destruct",    .token = KEYWORD,   .subToken = DESTRUCT 	        },
    {.data = "template",    .token = KEYWORD,	.subToken = TEMPLATE 	        },
    {.data = "import",      .token = KEYWORD,	.subToken = IMPORT 		},
    {.data = "export",      .token = KEYWORD,	.subToken = EXPORT 		},
	
    //Logical operators
    {.data = "not",     .token = LOGIC,         .subToken = LOGIC_NOT   },
    {.data = "and",     .token = LOGIC,         .subToken = LOGIC_AND   },
    {.data = "or",      .token = LOGIC,         .subToken = LOGIC_OR    },
    
    {.data = NULL,      .token = UNKNOWN,       .subToken = ERR	        } //end
};

Token puncts[] = {
    // punct operators
    {.data = "(",  .token = PUNCT,      .subToken = L_PAREN		},
    {.data = ")",  .token = PUNCT,      .subToken = R_PAREN 	},
    {.data = "[",  .token = PUNCT,      .subToken = L_BRACKET	},
    {.data = "]",  .token = PUNCT,      .subToken = R_BRACKET	},
    {.data = "{",  .token = PUNCT,      .subToken = L_KEY	   	},
    {.data = "}",  .token = PUNCT,      .subToken = R_KEY		},
    {.data = ".",  .token = PUNCT,      .subToken = DOT   		},
    {.data = ",",  .token = PUNCT,      .subToken = COMMA  	 	},
    {.data = ";",  .token = PUNCT,      .subToken = SEMICOLON	},
    {.data = ":",  .token = PUNCT,      .subToken = COLON		},
	
    {.data = NULL, .token = UNKNOWN,    .subToken = ERR	} //end
};
Token operators[] = {
    //unary operators
    {.data = "++", .token = UNARY, .subToken = UNARY_PLUS  },
    {.data = "--", .token = UNARY, .subToken = UNARY_MINUS },
	
    //arithmetics operators
    {.data = "+", .token = ARITH, .subToken = SUM 	},
    {.data = "-", .token = ARITH, .subToken = SUB 	},
    {.data = "*", .token = ARITH, .subToken = MULT 	},
    {.data = "/", .token = ARITH, .subToken = DIV 	},
    {.data = "%", .token = ARITH, .subToken = MOD 	},

    //rational operators
    {.data = "==", .token = RATIONAL, .subToken = EQU	},
    {.data = "!=", .token = RATIONAL, .subToken = NEQ	},
    {.data = ">",  .token = RATIONAL, .subToken = GRT	},
    {.data = ">=", .token = RATIONAL, .subToken = GRE	},
    {.data = "<",  .token = RATIONAL, .subToken = LST	},
    {.data = "<=", .token = RATIONAL, .subToken = LSE	},

    //unary operators
    {.data = "=",  .token = ASSIGN, .subToken = EQUAL    	},
    {.data = "+=", .token = ASSIGN, .subToken = SUM_EQUAL	},
    {.data = "-=", .token = ASSIGN, .subToken = SUB_EQUAL	},
    {.data = "*=", .token = ASSIGN, .subToken = MULT_EQUAL	},
    {.data = "/=", .token = ASSIGN, .subToken = DIV_EQUAL	},
    {.data = "%=", .token = ASSIGN, .subToken = MOD_EQUAL	},
	
    {.data = NULL, .token = UNKNOWN, .subToken = ERR	} //end	
};
