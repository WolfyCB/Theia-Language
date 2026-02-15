#include "lexer.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


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
        .next = 1,
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
    lexer->cur = lexer->next++;
    lexer->col++;
	if(lexer->cur >= lexer->codeLen) return '\0';
    return lexer->code[lexer->cur];
}
int peek(Lexer* lexer){
	if(lexer->cur >= lexer->codeLen) return '\0';
    return lexer->code[lexer->cur];
}
int next(Lexer* lexer){
	if(lexer->next >= lexer->codeLen) return '\0';
    return lexer->code[lexer->next];
}
void consume(Lexer* lexer){
    strapp(&lexer->buffer, lexer->code[lexer->cur]);
}
void fabricToken(Lexer* lexer, const Token token){
    char* data = strdup(token.data);
    if(!data)
	theia_error("Fail to duplicate string\n");
    vecadd(&lexer->vec, (Token){
        .data = data,
        .token = token.token,
        .subToken = token.subToken,
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
    while(strncmp(array[i].data, str.data, str.size) != 0){\
		if(array[i].token == UNKNOWN)\
			break;\
		i++;\
    }\
    return array[i];\
}
GETTOKEN(keywords);
GETTOKEN(puncts);
GETTOKEN(operators);
#undef GETTOKEN



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

int isIdentStart(Lexer** lexer){
	int cur = peek(*lexer);
    return isalpha(cur) || cur == '_';
}
void isIdent(Lexer** lexer){
	Token t = {
		.token = IDENT,
		.subToken = VAR_ID
	};
	int cur = peek(*lexer);
	while(isalnum(cur) || cur == '_' ){
		consume(*lexer);
		advance(*lexer);
	}
	Token key = get_keywords_token((*lexer)->buffer);
	
	if(key.token != UNKNOWN){
		t.token = key.token;
		t.subToken = key.subToken;
	}
	
	t.data = (*lexer)->buffer.data;
	fabricToken(*lexer, t);
}


int isNumberStart(Lexer** lexer){
	int nextDigit = isdigit(next(*lexer)); 
	return isdigit(peek(*lexer)) || (peek(*lexer) == '-' && nextDigit) || (peek(*lexer) == '.' && nextDigit);
}
void isNumber(Lexer** lexer){
	consume(*lexer);
	advance(*lexer);
	char isFloat = 0;
	
	while(isdigit(peek(*lexer)) || peek(*lexer) == '.'){
		if(peek(*lexer) == '.') {
			if(isFloat) break;
			isFloat = 1;
		}
		consume(*lexer);
		advance(*lexer);
	}
	Token t = {
		.data = (*lexer)->buffer.data,
		.token = VALUE,
	};
	t.subToken = (isFloat) ? FLOATING : INTEGER;

	fabricToken(*lexer, t);
}

void isChar(Lexer** lexer){
	advance(*lexer);
	consume(*lexer);
	advance(*lexer);
	if(peek(*lexer) != '\''){
		fabricToken(*lexer, (Token){
			.data = (*lexer)->buffer.data,
			.token = UNKNOWN,
			.subToken = 0
		});
		advance(*lexer);
		return;
	}
	
	fabricToken(*lexer, (Token){
		.data = (*lexer)->buffer.data,
		.token = VALUE,
		.subToken = CHARACTER
	});
	advance(*lexer);
}
void isString(Lexer** lexer){
	while(advance(*lexer)){
		if(peek(*lexer) == '\"' && peek(*lexer) != '\0') break;
		consume(*lexer);
	}
	if (peek(*lexer) != '\"'){
		fabricToken(*lexer, (Token){
			.data = (*lexer)->buffer.data,
			.token = UNKNOWN,
			.subToken = 0
		});
		advance(*lexer);
		return;
	}
	advance(*lexer);
	fabricToken(*lexer, (Token){
		.data = (*lexer)->buffer.data,
		.token = VALUE,
		.subToken = STRING
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
	while(advance(*lexer) != '\0')
		if(peek(*lexer) == ']' && next(*lexer) == '#'){
			advance(*lexer);
			advance(*lexer);
			break;
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
		.data = lexer->buffer.data,
		.token = PUNCT,
		.subToken = key.subToken
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
        .data = "eof\0",
        .token = END_OF_FILE,
        .subToken = 0
    });
}
