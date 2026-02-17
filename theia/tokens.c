#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>


int cmptoken(const Token a, const Token b){
    return a.Class == b.Class && a.type == b.type;
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



TokenElement keywords[] = {
    {.data = "true",	.Class = VALUE,         .type = TRUE_BOOL	},
    {.data = "false",	.Class = VALUE,         .type = FALSE_BOOL	},

    //data types
    {.data = "void",        .Class = DATA_TYPE,	.type = VOID		},
    {.data = "let",         .Class = DATA_TYPE,	.type = LET			},
    {.data = "char",        .Class = DATA_TYPE,	.type = CHAR		},
    {.data = "uchar",       .Class = DATA_TYPE,	.type = UCHAR		},
    {.data = "str",         .Class = DATA_TYPE,	.type = STR			},
    {.data = "bool",        .Class = DATA_TYPE,	.type = BOOL		},
    {.data = "int",         .Class = DATA_TYPE,	.type = INT			},
    {.data = "uint",        .Class = DATA_TYPE,	.type = UINT		},
    {.data = "float",       .Class = DATA_TYPE,	.type = FLOAT		},
    {.data = "double",      .Class = DATA_TYPE,	.type = DOUBLE		},
    {.data = "enum",        .Class = DATA_TYPE,	.type = ENUM		},
    {.data = "short",       .Class = DATA_TYPE,	.type = SHORT		},
    {.data = "long",        .Class = DATA_TYPE,	.type = LONG		},
    //control keywords
    {.data = "end",         .Class = CONTROL,   .type = END 		},
    {.data = "if",          .Class = CONTROL,   .type = IF 			},
    {.data = "elif",        .Class = CONTROL,   .type = ELIF 		},
    {.data = "else",        .Class = CONTROL,   .type = ELSE 		},
    {.data = "for",         .Class = CONTROL,   .type = FOR 		},
    {.data = "while",       .Class = CONTROL,   .type = WHILE 		},
    {.data = "do",       	.Class = CONTROL,   .type = DO 		},
    {.data = "switch",      .Class = CONTROL,   .type = SWITCH 		},
    {.data = "return",      .Class = CONTROL,   .type = RETURN 		},
    {.data = "continue",    .Class = CONTROL,   .type = CONTINUE 	        },
    {.data = "break",       .Class = CONTROL,   .type = BREAK 		},
	//keywords
    {.data = "fn",          .Class = KEYWORD,	.type = FN 			},
    {.data = "object",      .Class = KEYWORD, 	.type = OBJECT 		},
    {.data = "construct",   .Class = KEYWORD,   .type = CONSTRUCT 	        },
    {.data = "destruct",    .Class = KEYWORD,   .type = DESTRUCT 	        },
    {.data = "template",    .Class = KEYWORD,	.type = TEMPLATE 	        },
    {.data = "import",      .Class = KEYWORD,	.type = IMPORT 		},
    {.data = "export",      .Class = KEYWORD,	.type = EXPORT 		},
	
    //Logical operators
    {.data = "not",     .Class = LOGIC,         .type = LOGIC_NOT   },
    {.data = "and",     .Class = LOGIC,         .type = LOGIC_AND   },
    {.data = "or",      .Class = LOGIC,         .type = LOGIC_OR    },
    
    {.data = NULL,      .Class = UNKNOWN,       .type = ERR	        } //end
};

TokenElement puncts[] = {
    // punct operators
    {.data = "(",  .Class = PUNCT,      .type = L_PAREN		},
    {.data = ")",  .Class = PUNCT,      .type = R_PAREN 	},
    {.data = "[",  .Class = PUNCT,      .type = L_BRACKET	},
    {.data = "]",  .Class = PUNCT,      .type = R_BRACKET	},
    {.data = "{",  .Class = PUNCT,      .type = L_KEY	   	},
    {.data = "}",  .Class = PUNCT,      .type = R_KEY		},
    {.data = ".",  .Class = PUNCT,      .type = DOT   		},
    {.data = ",",  .Class = PUNCT,      .type = COMMA  	 	},
    {.data = ";",  .Class = PUNCT,      .type = SEMICOLON	},
    {.data = ":",  .Class = PUNCT,      .type = COLON		},
    {.data = "?",  .Class = PUNCT,      .type = QUESTION	},
	
    {.data = NULL, .Class = UNKNOWN,    .type = ERR	} //end
};
TokenElement operators[] = {
    //unary operators
    {.data = "++", .Class = UNARY, .type = UNARY_PLUS  },
    {.data = "--", .Class = UNARY, .type = UNARY_MINUS },
	
    //arithmetics operators
    {.data = "+", .Class = ARITH, .type = SUM 	},
    {.data = "-", .Class = ARITH, .type = SUB 	},
    {.data = "*", .Class = ARITH, .type = MULT 	},
    {.data = "/", .Class = ARITH, .type = DIV 	},
    {.data = "%", .Class = ARITH, .type = MOD 	},

    //rational operators
    {.data = "==", .Class = RATIONAL, .type = EQU	},
    {.data = "!=", .Class = RATIONAL, .type = NEQ	},
    {.data = ">",  .Class = RATIONAL, .type = GRT	},
    {.data = ">=", .Class = RATIONAL, .type = GRE	},
    {.data = "<",  .Class = RATIONAL, .type = LST	},
    {.data = "<=", .Class = RATIONAL, .type = LSE	},

    //unary operators
    {.data = "=",  .Class = ASSIGN, .type = EQUAL    	},
    {.data = "+=", .Class = ASSIGN, .type = SUM_EQUAL	},
    {.data = "-=", .Class = ASSIGN, .type = SUB_EQUAL	},
    {.data = "*=", .Class = ASSIGN, .type = MULT_EQUAL	},
    {.data = "/=", .Class = ASSIGN, .type = DIV_EQUAL	},
    {.data = "%=", .Class = ASSIGN, .type = MOD_EQUAL	},
	
    {.data = NULL, .Class = UNKNOWN, .type = ERR	} //end	
};
