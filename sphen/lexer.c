#include "lexer.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>

void theia_error(const char* msg){
    printf("%s\n", msg);
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
    initTokens();
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
int peek(Lexer* lexer);
int next(Lexer* lexer);
inline int peek(Lexer* lexer){
    return (lexer->cur >= lexer->codeLen) ? '\0' : lexer->code[lexer->cur];
}
inline int next(Lexer* lexer){
    return (lexer->cur + 1 >= lexer->codeLen) ? '\0' : lexer->code[lexer->cur + 1];
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
    //printf("%s\n", lexer->buffer.data);
    strclean(&lexer->buffer);
}


int isPunct(const char c){
	if(!c) return 0;
    return strchr("(){}[].,;:?_", c) != NULL;
}
int isOperator(const char c){
	if(!c) return 0;
    return strchr("+-/*%=!<>", c) != NULL;
}

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
		t->data.f = num;
		return;
	}
}


int isIdentStart(Lexer** lexer){
	int cur = peek(*lexer);
	return isalpha(cur) || cur == '_';
}
void isIdent(Lexer** lexer){
	int cur = peek(*lexer);
	while(isalnum(cur) || cur == '_'){
		consume(*lexer);
		cur = advance(*lexer);
	}
	if((*lexer)->buffer.size == 1 && (*lexer)->buffer.data[0] == '_'){
			fabricToken(*lexer, getPunctToken("_", 1));
			return;
		}
	
	Token t = getKeywordToken((*lexer)->buffer.data, (*lexer)->buffer.size);
	switch(t.type){
		case BOOL_TRUE: t.data.b = 1; break;
		case BOOL_FALSE: t.data.b = 0; break;
		default: break;
	}
	fabricToken(*lexer, t);
}

void getOperator(Lexer** lexer){
	consume(*lexer);
	if(isOperator(next(*lexer))){
		advance(*lexer);
		consume(*lexer);
	}
	Token op = getOperatorToken((*lexer)->buffer.data, (*lexer)->buffer.size);
	
	fabricToken(*lexer, op);
	advance(*lexer);
}

int isNumberStart(Lexer** lexer){
	int nextDigit = isdigit(next(*lexer)); 
	int cur = peek(*lexer);
	return isdigit(cur) || (cur == '.' && nextDigit);
}
void isNumber(Lexer** lexer){
	char isFloat = 0;
	char c = peek(*lexer);
	do {
		if(c == '.') {
			if(isFloat) break;
			isFloat = 1;
		}
		consume(*lexer);
		c = advance(*lexer);
	} while(isdigit(c) || c == '.');
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
	int c = peek(lexer);
	while(c){
	if(lexer->cur >= lexer->codeLen) break;
	c = peek(lexer);
	// ignore whitespaces
	while(isspace(c) && c != '\0'){
		c = advance(lexer);
	}
    
	// comments
	if(c == '#' && next(lexer) == '['){
	    ignoreMultiline(&lexer);
	    continue;
	}
	if(c == '#'){
		ignoreLine(&lexer);
		continue;
	}
        
	// idents and keywords
	if(isIdentStart(&lexer)){
		isIdent(&lexer);
		continue;
	}
        
	//char
	if(c == '\''){
	    isChar(&lexer);
	    continue;
	}

	//string
	if(c == '"'){
	    isString(&lexer);
	    continue;
	}
	
        //numbers
        if(isNumberStart(&lexer)){
	    isNumber(&lexer);
            continue;
        }
	if(isPunct(c)){
		consume(lexer);
	    advance(lexer);
		if(c == '.' && peek(lexer)=='.'){
			consume(lexer);
	    	advance(lexer);
	    	if(peek(lexer) == '=')
				consume(lexer);
		}
		fabricToken(lexer, getPunctToken(lexer->buffer.data, lexer->buffer.size));
    	advance(lexer);
		
	    continue;
	}
	if(isOperator(c)){
	    getOperator(&lexer);
	    continue;
	}
	break;
    }
    fabricToken(lexer, (Token){
        .data.b = 0,
        .Class = END_OF_FILE,
        .type = 0
    });
}
