#include "lexer.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>

void theia_error(const char* msg){
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}

Lexer lexer_init(const char* filename){
    FILE* f = fopen(filename, "rb");
    if(!f)
        theia_error("Can not read/open file\n");
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    char* codeContent = malloc(size+1);
    if(!codeContent)
	theia_error("Code memory alloc fail\n");
    fread(codeContent, 1, size, f);
    codeContent[size] = '\0';
    fclose(f);
    
    return (Lexer){
        .cur = 0,
        .col = 0,
        .line = 0,
        .code = codeContent,
        .codeLen = size+1,
        .buffer = newstr("\0"),
        .vec = newvec()
    };

}
void lexer_close(Lexer* lexer){
    free(lexer->code);
    lexer->code = NULL;
    vecdel(&lexer->vec);
    strdel(&lexer->buffer);
}
int advance(Lexer* lexer){
    if(lexer->cur >= lexer->codeLen) return '\0';
	
    if(lexer->code[lexer->cur] == '\n'){
        lexer->col = 0;
        lexer->line++;
    }
    lexer->cur++;
    lexer->col++;
    return lexer->code[lexer->cur];
}
int peek(Lexer* lexer){
    return lexer->code[lexer->cur];
}
int next(Lexer* lexer){
    if(lexer->cur + 1 >= lexer->codeLen) return '\0';
    return lexer->code[lexer->cur + 1];
}
void consume(Lexer* lexer){
    strapp(&lexer->buffer, lexer->code[lexer->cur]);
}
void fabricToken(Lexer* lexer, const Token token){
	vecadd(&lexer->vec, (Token){
        .data = token.data,
        .Class = token.Class,
        .type = token.type,
        .line = lexer->line,
        .col = lexer->col
    });
    strclean(&lexer->buffer);
}


int isPunct(const char c){
    return strchr("(){}[]<>.,;:", c) != NULL;
}
int isOperator(const char c){
    return strchr("+-=/%!*&?", c) != NULL;
}

#define GETTOKEN(array)\
Token get_##array##_token(const String str){\
    int i = 0;\
    Token t = {\
    	.Class = UNKNOWN,\
    	.type = ERR\
    };\
    while(strncmp(array[i].data, str.data, str.size) != 0){\
		if(array[i].Class == UNKNOWN){\
			t.data.str = strdup(array[i].data);\
			break;\
		}\
		i++;\
    }\
    if(array[i].Class){\
    	t.Class = array[i].Class;\
    	t.type = array[i].type;\
    }\
    return t;\
}
GETTOKEN(keywords);
GETTOKEN(puncts);
GETTOKEN(operators);
#undef GETTOKEN

void typeStrConvert(Token* t, const char* str){
	char* endptr;
	if(t->type == INTEGER){
		unsigned long long ul = strtoull(str, &endptr, 10); 
		if(str[0] != '-' && ul > LONG_MAX ){
			t->data.u = ul;
			return;
		}
		t->data.i = strtoll(str, &endptr, 10);
		return;
	}
	if(t->type == FLOATING){
		double num = strtod(str, &endptr);
		if(num > FLT_MAX || num < FLT_MIN){
			t->data.d = num;
			return;
		}
		t->data.f = num;
		return;
	}
}


int isIdentStart(Lexer** lexer){
	int cur = peek(*lexer);
    return isalpha(cur) || cur == '_';
}
void isIdent(Lexer** lexer){
	Token t = {
		.Class = IDENT,
		.type = VAR_ID
	};
	int cur = peek(*lexer);
	while(isalnum(cur) || cur == '_' ){
		consume(*lexer);
		cur = advance(*lexer);
	}
	Token key = get_keywords_token((*lexer)->buffer);
	if(key.Class != UNKNOWN){
		t.Class = key.Class;
		t.type = key.type;
	}
	t.data.str = key.data.str;
	if(key.type == TRUE_BOOL) t.data.b = 1;
	if(key.type == FALSE_BOOL) t.data.b = 0;
	fabricToken(*lexer, t);
}

void getOperator(Lexer** lexer){
	consume(*lexer);
	if(next(*lexer) != '\0' && isOperator(next(*lexer))){
		advance(*lexer);
		consume(*lexer);
	}
	advance(*lexer);
	
	Token key = get_operators_token((*lexer)->buffer);
	fabricToken(*lexer, key);
}

int isNumberStart(Lexer** lexer){
	int nextDigit = isdigit(next(*lexer)); 
	int cur = peek(*lexer);
	return isdigit(cur) || (cur == '-' && nextDigit) || (cur == '.' && nextDigit);
}
void isNumber(Lexer** lexer){
	consume(*lexer);
	advance(*lexer);
	char isFloat = 0;
	char c = peek(*lexer);
	while(isdigit(c) || c == '.'){
		if(peek(*lexer) == '.') {
			if(isFloat) break;
			isFloat = 1;
		}
		consume(*lexer);
		c = advance(*lexer);
	}
	Token t = {
		.Class = VALUE,
	};
	t.type = (isFloat) ? FLOATING : INTEGER;
	typeStrConvert(&t, (*lexer)->buffer.data);
	fabricToken(*lexer, t);
}

void isChar(Lexer** lexer){
	advance(*lexer);
	consume(*lexer);
	advance(*lexer);
	if(peek(*lexer) != '\''){
		fabricToken(*lexer, (Token){
			.Class = UNKNOWN,
			.type = ERR
		});
		advance(*lexer);
		return;
	}
	Token t = {
		.Class = VALUE,
		.type = CHARACTER
	};
	unsigned char uc = (*lexer)->buffer.data[0];
	char c = (*lexer)->buffer.data[0];
	
	if(uc > 127) t.data.uc = uc; 
	else t.data.c = c;
	
	fabricToken(*lexer, t);
	advance(*lexer);
}
void isString(Lexer** lexer){
	char c = advance(*lexer);
	while(c != '\0'){
		if(c == '"') break;
		consume(*lexer);
		c = advance(*lexer);
	}
	if (peek(*lexer) != '"'){
		fabricToken(*lexer, (Token){
			.Class = UNKNOWN,
			.type = ERR
		});
		return;
	}
	advance(*lexer);
	fabricToken(*lexer, (Token){
		.data.str = (*lexer)->buffer.data,
		.Class = VALUE,
		.type = STRING
	});
}

int isCommentLine(Lexer** lexer){
    return peek(*lexer)=='#';
}
void ignoreLine(Lexer** lexer){
	while(advance(*lexer) != '\0')
		if(peek(*lexer) == '\n'){
			advance(*lexer);
			break;
		}
}
int isCommentMultiline(Lexer** lexer){
	return peek(*lexer)=='#' && next(*lexer)=='[';
}
void ignoreMultiline(Lexer** lexer){
	char c = peek(*lexer);
        while(c != '\0'){
		if(c == ']')
                    if(next(*lexer) == '#'){
			advance(*lexer);
			advance(*lexer);
			break;
		    }
		c = advance(*lexer);
	}
}



void tokenize(Lexer* lexer) {
	while(peek(lexer) != '\0'){
	// ignore whitespaces
	while(isspace(peek(lexer)))
		advance(lexer);
    
	// comments
	if(isCommentMultiline(&lexer)){
	    ignoreMultiline(&lexer);
	    continue;
	}
	if(isCommentLine(&lexer)){
		ignoreLine(&lexer);
		continue;
	}
        
	// idents and keywords
	if(isIdentStart(&lexer)){
		isIdent(&lexer);
		continue;
	}
        
	//char
	if(peek(lexer) == '\''){
	    isChar(&lexer);
	    continue;
	}

	//string
	if(peek(lexer) == '"'){
	    isString(&lexer);
	    continue;
	}
						
        //numbers
        if(isNumberStart(&lexer)){
	    isNumber(&lexer);
            continue;
        }
	if(isPunct(peek(lexer))){
	    consume(lexer);
	    Token key = get_puncts_token(lexer->buffer);
	    fabricToken(lexer, (Token){
		.data.c = 0,
		.Class = PUNCT,
		.type = key.type
	    });
	    advance(lexer);
	    continue;
	}
	if(isOperator(peek(lexer))){
	    getOperator(&lexer);
	    continue;
	}
	advance(lexer);
    }
    fabricToken(lexer, (Token){
        .data.b = 0,
        .Class = END_OF_FILE,
        .type = 0
    });
}
