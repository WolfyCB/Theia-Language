#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include "parser.h"
#include "debug.h"

#define TRUE 1
#define FALSE 0
#define _NODE(parser, node) parser->nodes.data[node.id]

#define ERROR 1
#define WARN 2

void newdebug(ParserDebug* debug){
    debug->len = 0;
    debug->cap = 8;
    debug->data = (DebugMessage*)calloc(debug->cap, sizeof(DebugMessage));
    if(!debug->data){
        printf("DebugMessage does not exist\n");
        exit(EXIT_FAILURE);	
    }
}
void debugadd(ParserDebug* debug, const char* message, unsigned long line, unsigned long col){
    if(!debug || !debug->data){
        printf("DebugMessage does not exist\n");
        exit(EXIT_FAILURE);
        
    }
    if(debug->len >= debug->cap){
        debug->cap += 8;
        debug->data = (DebugMessage*)realloc(debug->data, debug->cap * sizeof(DebugMessage)); 
        if(!debug->data){
            printf("DebugMessage memory allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
    debug->data[debug->len++] = (DebugMessage){.message = message, .line = line, .col = col};
}
void deldebug(ParserDebug* debug){
    free(debug->data);
    debug->data = NULL;
    debug->len = debug->cap = 0;
}
void parse_debug(Parser* p, const unsigned char type, const char* message){
    switch(type){
        case ERROR: 
            //LOG("$<red, clear, bold:E>$: %s | Class: %d Type: %d\n", message, p->lexer.vec.data[p->pos].Class, p->lexer.vec.data[p->pos].type);
            debugadd(&p->errors, message, p->lexer.vec.data[p->pos].line, p->lexer.vec.data[p->pos].col); 
            break;
        case WARN: 
            //LOG("$<yellow, clear, bold:W>$: %s\n", message);
            debugadd(&p->warns, message, p->lexer.vec.data[p->pos].line, p->lexer.vec.data[p->pos].col); 
            break;
    }
}
void show_debug(Parser* p){
    unsigned long i = 0;
    while(i < p->errors.len){
        LOG("at line: %u, col: %u", p->errors.data[i].line, p->errors.data[i].col);
        LOG("$<red, clear, bold:E>$: %s\n", p->errors.data[i].message);
        i++;
    }
    i=0;
    while(i < p->warns.len){
        LOG("at line: %u, col: %u", p->warns.data[i].line, p->warns.data[i].col);
        LOG("$<yellow, clear, bold:W>$: %s\n", p->warns.data[i].message);
        i++;
    }

    int haveErr = (p->errors.len) ? 1 : 0;
    if(p->errors.len) LOG("$<red, clear, bold:Errors>$:%d", p->errors.len);
    if(p->warns.len) LOG("$<yellow, clear, bold:Warns>$:%d",p->warns.len);

    deldebug(&p->errors);
    deldebug(&p->warns);

    if(haveErr) exit(EXIT_FAILURE);
}


//forward declaration
NodeId parse_getValue(Parser* p);
NodeId parse_expr(Parser* p, int prec);
NodeId parse_tuple_expr(Parser* p);

ASTblock parse_block(Parser* p, TokenType endToken, char* err);

NodeId parse_type(Parser* p);
NodeId parse_let_decl(Parser* p);
NodeId parse_const_decl(Parser* p);
NodeId parse_func_decl(Parser* p);

// Nodeid parse_object_decl(Parser* p);
// 
// NodeId parse_var_call(Parser* p);
// NodeId parse_func_call(Parser* p);
// NodeId parse_call(Parser* p);
// 
NodeId parse_if_statement(Parser* p);
// NodeId parse_for_statement(Parser* p);
NodeId parse_while_statement(Parser* p);
NodeId parse_do_while_statement(Parser* p);
NodeId parse_switch_statement(Parser* p);

NodeId parse_return_statement(Parser* p);
NodeId parse_single_statement(Parser* p, ASTType type);
// NodeId parse_import_statement(Parser* p);
// NodeId parse_export_statement(Parser* p);


ASTNode parse_node(Parser* p, ASTType type);
Token parse_peek(Parser* p);
Token parse_next(Parser* p);
int parse_isEof(Parser* p);
int matchToken(Parser* p, TokenClass Class, TokenType type);
int matchClass(Parser* p, TokenClass Class);
int matchType(Parser* p, TokenType type);
int parse_program(Parser* p);


Parser parser_init(const char* filename){
    Parser p;
    
    newastvec(&p.nodes);
    p.id = 0;               // INICIALIZAÇÃO ADICIONADA
    p.pos = 0;
    p.count = 0;
    newdebug(&p.errors);
    newdebug(&p.warns);

    p.lexer = lexer_init(filename);
    return p;
}
void parser_close(Parser* p){
    lexer_close(&p->lexer);
    delast(&p->nodes);
}
int parse(Parser* p){
    tokenize(&p->lexer);
    /*
    printf("\ntokens:\n");
    Token tok;
    unsigned int line = 0;
    for(unsigned long int i = 0; i < p->lexer.vec.size; i++){
        tok = p->lexer.vec.data[i];
        if(line != tok.line){
            line = tok.line;
            printf("\n");
            for(unsigned int col = 0; col < tok.col; col++ ) printf(" ");
        }
        printf("%02d ", tok.type);
    }
    */
    parse_program(p);
    show_debug(p);
    return 0;
}

inline ASTNode parse_node(Parser* p, ASTType type){
    //printf("new node l:%02d | id:%03llu type:%02d\n", parse_peek(p).line, p->id, type);
    return (ASTNode){.type = type, .id = p->id++};
}
inline int parse_isEof(Parser* p){
    return !(p->pos < p->lexer.vec.size) || p->lexer.vec.data[p->pos].Class == END_OF_FILE;
}
inline Token parse_peek(Parser* p){
    //printf("peek: %d\n", p->lexer.vec.data[p->pos].type);
    return (p->pos < p->lexer.vec.size) ? p->lexer.vec.data[p->pos] : (Token){.Class = END_OF_FILE};
}
inline Token parse_next(Parser* p){
    //if(p->lexer.vec.data[p->pos].Class)
    //	printf("next: %d\n", p->lexer.vec.data[p->pos].type);
    return (p->pos < p->lexer.vec.size) ? p->lexer.vec.data[p->pos++] : (Token){.Class = END_OF_FILE};
}
inline int matchToken(Parser* p, TokenClass Class, TokenType type){
    return parse_peek(p).Class == Class && parse_peek(p).type == type && !parse_isEof(p);
}
inline int matchType(Parser* p, TokenType type){
    return parse_peek(p).type == type && !parse_isEof(p);
}
inline int matchClass(Parser* p, TokenClass Class){
    return parse_peek(p).Class == Class && !parse_isEof(p);
}
Token expectToken(Parser* p, TokenClass Class, TokenType type, const char* err){
    if(matchToken(p, Class, type)) return parse_next(p);
    parse_debug(p, ERROR, err);
    return parse_peek(p);
}
Token expectClass(Parser* p, TokenClass Class, const char* err){
    if(matchClass(p, Class)) return parse_next(p);
    parse_debug(p, ERROR, err);
    return parse_peek(p);
}
Token expectType(Parser* p, TokenType type, const char* err){
    if(matchType(p, type)) return parse_next(p);
    parse_debug(p, ERROR, err);
    return parse_peek(p);
}

int get_precedence(TokenType type) {
    switch(type) {
        case SUM_EQUAL:
        case SUB_EQUAL:
        case DIV_EQUAL:
        case MULT_EQUAL:
        case MOD_EQUAL:
        case EQUAL: return PREC_ASSIGN;
        case LOGIC_OR: return PREC_OR;
        case LOGIC_AND: return PREC_AND;
        case EQU:
        case NEQ: return PREC_EQUALITY;
        case GRE:
        case LSE:
        case LST:
        case GRT: return PREC_COMPARE;
        case SUM:
        case SUB: return PREC_TERM;
        case MOD:
        case MULT:
        case DIV: return PREC_FACTOR;
        default: return PREC_NONE;
    }
}
NodeId parse_primary(Parser* p){
    Token t = parse_next(p);
    ASTNode node;
    if(t.Class == VALUE){
            node = parse_node(p, AST_LITERAL);
            node.literal.type = t.type;
            node.literal.data = t.data;
            pushnode(&p->nodes, node);
            return node.id;
    }
    if(t.type == L_PAREN){
            node.id = parse_expr(p, parse_peek(p).type);
            expectToken(p, PUNCT, R_PAREN, "expected a$<cyan clear bold: )>$ to close expression");
            return node.id;
    }
    if(t.type == SUB || t.type == LOGIC_NOT){
        NodeId right = parse_expr(p, PREC_UNARY);

        node = parse_node(p, AST_EXPR);
        node.expr.left = 0;
        node.expr.op = t.type;
        node.expr.right = right;
        pushnode(&p->nodes, node);
        return node.id;
        
    }
    parse_debug(p, ERROR, "expected some $<cyan clear bold:value>$ to expression");
    return node.id;
}

NodeId parse_expr(Parser* p, int prec){
    NodeId left = parse_primary(p);
    while (prec < get_precedence(parse_peek(p).type)) {
        Token op = parse_next(p);
        int next_prec = get_precedence(op.type);

        if(op.type == EQUAL) next_prec--;
        
        ASTNode node = parse_node(p, AST_EXPR);
        pushnode(&p->nodes, node);
        NodeId right = parse_expr(p, next_prec);
        _NODE(p, node).expr.left = left;
        _NODE(p, node).expr.op = op.type;
        _NODE(p, node).expr.right = right;
        left = node.id;
    }

    return left;	
}
NodeId parse_tuple_expr(Parser* p){
    parse_next(p);
    unsigned char expect_comma = FALSE;

    ASTNode node = parse_node(p, AST_TUPLE_EXPR);
    pushnode(&p->nodes, node);
    _NODE(p, node).tuple.values.begin = p->nodes.len;

    while(TRUE){
        if(expect_comma){
            if(expectToken(p, PUNCT, COMMA, "expected a $<cyan clear bold:,>$ to set new value to $<cyan clear bold:tuple>$").type == COMMA)
                expect_comma = FALSE;
            else
                break;
        }
        if(matchToken(p, PUNCT, R_PAREN) || matchToken(p, PUNCT, COMMA)){ 
            parse_debug(p, ERROR, "expected a $<cyan clear bold:value>$ at $<cyan clear bold:tuple>$"); 
            parse_next(p);
            break;
        }
        
        parse_getValue(p);
        _NODE(p, node).tuple.count++;
        if(matchToken(p, PUNCT, COMMA)) expect_comma = TRUE;
        else break;
    }

    _NODE(p, node).tuple.values.count = p->nodes.len - _NODE(p, node).tuple.values.begin;
    if(_NODE(p, node).tuple.count < 2) parse_debug(p, ERROR, "$<cyan clear bold:tuples>$ needs $<cyan clear bold:2 or more>$ values");
    expectToken(p, PUNCT, R_PAREN, "expected a $<cyan clear bold:)>$ to close $<cyan clear bold:tuple>$");
    return _NODE(p, node).id;
}
NodeId parse_getValue(Parser* p){
    if(matchToken(p, PUNCT, L_PAREN)) {
        int i = p->pos;
        unsigned char isTuple = FALSE;
        while(p->lexer.vec.data[i].type != R_PAREN){
            if(p->lexer.vec.data[i].type == COMMA ){isTuple=TRUE; break;}
            i++;
        }
        if(isTuple) return parse_tuple_expr(p);
    }
    
    return parse_expr(p, get_precedence(parse_peek(p).type));
}


NodeId parse_getAST(Parser* p){
    switch(parse_peek(p).type){
        case 	  LET:	return parse_let_decl(p);
        case  	CONST:	return parse_const_decl(p);
        case 	   FN:	return parse_func_decl(p);
        //case OBJECT:	return parse_object_decl(p);
        case 	   IF:	return parse_if_statement(p);
        //case 	FOR:	return parse_for_statement(p);
        case   SWITCH:	return parse_switch_statement(p);
        case 	WHILE:	return parse_while_statement(p); 
        case 	   DO:	return parse_do_while_statement(p);
        case   RETURN:	return parse_return_statement(p);
        case 	BREAK:	return parse_single_statement(p, AST_BREAK_STMT);
        case CONTINUE:	return parse_single_statement(p, AST_CONTINUE_STMT);
        default: parse_debug(p, ERROR, "command not found"); parse_next(p); break;
    }
}
inline int parse_program(Parser* p){
    while(!parse_isEof(p))
        parse_getAST(p);
    
    return 0;
}


NodeId parse_return_statement(Parser* p){
    ASTNode node = parse_node(p, AST_RETURN_STMT);
    pushnode(&p->nodes, node);
    parse_next(p);
    _NODE(p, node).return_st.expr = parse_getValue(p);
    return node.id;
}
NodeId parse_single_statement(Parser* p, ASTType type){
    ASTNode node = parse_node(p, type);
    pushnode(&p->nodes, node);
    parse_next(p);
    return node.id;
}

NodeId parse_tuple_type(Parser* p){
    parse_next(p);
    ASTNode node = parse_node(p, AST_TUPLE_TYPE);
    pushnode(&p->nodes, node);
    
    unsigned char expect_comma = FALSE;
    _NODE(p, node).tuple.values.begin = p->nodes.len;
    while(TRUE){
        if(expect_comma){
            if(expectToken(p, PUNCT, COMMA, "expected a $<cyan clear bold:,>$ to set new type to $<cyan clear bold:tuple type>$").type == COMMA)
                expect_comma = FALSE;
            else
                break;
        }
        if(matchToken(p, PUNCT, R_PAREN) || matchToken(p, PUNCT, COMMA)){ 
            parse_debug(p, ERROR, "expected a $<cyan clear bold:type>$ at $<cyan clear bold:tuple type>$"); 
            parse_next(p);
            break;
        }

        parse_type(p);
        _NODE(p, node).tuple.count++;
        if(matchToken(p, PUNCT, COMMA)) expect_comma = TRUE;
        else break;
    }
    _NODE(p, node).tuple.values.count = p->nodes.len - _NODE(p, node).tuple.values.begin;
    if(_NODE(p, node).tuple.count < 2) parse_debug(p, ERROR, "$<cyan clear bold:tuple types>$ needs $<cyan clear bold:2 or more>$ types");
    expectToken(p, PUNCT, R_PAREN, "expected a $<cyan clear bold:)>$ to close $<cyan clear bold:tuple type>$");
    return node.id;
}
NodeId parse_single_type(Parser* p){
    ASTNode node = parse_node(p, AST_TYPE);
    pushnode(&p->nodes, node);
    _NODE(p, node).data_type.isId = (parse_peek(p).Class == IDENT) ? TRUE : FALSE;

    if(_NODE(p, node).data_type.isId){
    //	_NODE(p, node).data_type.typeId = parse_peek(p).data.str;
        parse_next(p);
        printf("we dont have id type yet\n");
    }else
        _NODE(p, node).data_type.typeP = expectClass(p, DATA_TYPE, "expected some $<cyan clear bold:type>$").type;
    
    return node.id;
}

NodeId parse_type(Parser* p){
    if(parse_peek(p).Class == DATA_TYPE || parse_peek(p).Class == IDENT)
        return parse_single_type(p);
    if(parse_peek(p).type == L_PAREN)	
        return parse_tuple_type(p);
    parse_debug(p, ERROR, "expected a $<cyan clear bold:type>$ after $<cyan clear bold::>$");
    return 0;
}
NodeId parse_const_decl(Parser* p){
    ASTNode node = parse_node(p, AST_VAR_DECL);
    pushnode(&p->nodes, node);
    _NODE(p, node).var.attr = ATTR_CONST;

    parse_next(p);
    if(matchToken(p, PUNCT, UNDERLINE)) parse_debug(p, ERROR, "expected a letter or digit after $<cyan clear bold:_>$ at constant variable declaration");
    _NODE(p, node).var.name = expectToken(p, IDENT, ID, "expected a $<cyan clear bold:name>$ to variable declaration").data.str;
    
    if(matchToken(p, PUNCT, COLON)){
        parse_next(p);
        _NODE(p, node).var.type = parse_type(p);
    }
    expectToken(p, ASSIGN, EQUAL, "expected a$<cyan clear bold: = >$to assign a value to constant variable");
    _NODE(p, node).var.value = parse_getValue(p);
    return node.id;
}
NodeId parse_let_decl(Parser* p){
    ASTNode node = parse_node(p, AST_VAR_DECL);
    pushnode(&p->nodes, node);
    _NODE(p, node).var.attr = ATTR_LET;
    
    parse_next(p);
    if(matchToken(p, PUNCT, UNDERLINE)){
        parse_debug(p, ERROR, "expected a letter or digit after $<cyan clear bold:_>$ at let variable declaration");
        parse_next(p);
    }
    else _NODE(p, node).var.name = expectToken(p, IDENT, ID, "expected a $<cyan clear bold:name>$ to variable declaration").data.str;

    if(matchToken(p, PUNCT, COLON)){
        parse_next(p);
        _NODE(p, node).var.type = parse_type(p);
    }
    int equal = matchToken(p, ASSIGN, EQUAL);
    if(!_NODE(p, node).var.type) expectToken(p, ASSIGN, EQUAL, "expected a $<cyan clear bold:explicit type>$ or$<cyan clear bold: = >$to create variable");
    
    if(equal){
        if(_NODE(p, node).var.type) parse_next(p);
        _NODE(p, node).var.value = parse_getValue(p);
    }
    return node.id;
}

ASTblock parse_block(Parser* p, TokenType endToken, char* err){
    ASTblock block;
    block.begin = p->nodes.len;
    block.last = 0;  // inicializa para evitar lixo
    while(parse_peek(p).type != endToken && !parse_isEof(p))
        block.last = parse_getAST(p);
    block.count = p->nodes.len - block.begin;
    expectType(p, endToken, err);
    return block;
}

void parse_func_params(Parser* p){
    int first = TRUE;
    while(parse_peek(p).type != R_PAREN && !parse_isEof(p)){
        //TMP_DEBUG("arging")
        if(!first) expectToken(p, PUNCT, COMMA, "expected $<cyan clear bold:,>$ between parameters");
        first = FALSE;

        ASTNode node = parse_node(p, AST_FN_PARAM);
        pushnode(&p->nodes, node);
        
        switch(parse_peek(p).type){
            case REF: _NODE(p, node).param.attr = ATTR_REF; parse_next(p); break;
            case CONST: _NODE(p, node).param.attr = ATTR_CONST; parse_next(p); break;
            default: _NODE(p, node).param.attr = ATTR_LET; break;
        } 
        
        _NODE(p, node).param.name = expectClass(p, IDENT, "expected a parameter $<cyan clear bold:name>$").data.str;
        
        expectToken(p, PUNCT, COLON, "require a $<cyan clear bold:data type>$ for parameter");
        _NODE(p, node).param.type = parse_type(p);
        
        if(matchToken(p, ASSIGN, EQUAL)){
            parse_next(p);
            _NODE(p, node).param.std_value = parse_getValue(p);
        }
    }
    parse_next(p);
}


NodeId parse_func_decl(Parser* p){
    ASTNode node = parse_node(p, AST_FN_DECL);
    pushnode(&p->nodes, node);
    
    parse_next(p);
    if(matchToken(p, PUNCT, UNDERLINE)){ 
        parse_debug(p, ERROR, "expected a letter or digit after $<cyan clear bold:_>$ at function declaration");
        parse_next(p);
    } else _NODE(p, node).func.name = expectClass(p, IDENT, "expected function $<cyan clear bold>name").data.str;
    expectToken(p, PUNCT, L_PAREN, "expected a $<cyan clear bold:(>$ to open args");
    
    _NODE(p, node).func.param.begin = p->nodes.len;
    parse_func_params(p);
    _NODE(p, node).func.param.count = p->nodes.len - _NODE(p, node).func.param.begin;
    
    _NODE(p, node).func.ret_type = VOID;
    if(matchToken(p, PUNCT, COLON)){
        parse_next(p);
        _NODE(p, node).func.ret_type = parse_type(p);
    }
    _NODE(p, node).func.block = parse_block(p, END, "expected a $<cyan, clear, bold:end>$ to close function");
    return node.id;
}

NodeId parse_else_statement(Parser* p){
    ASTNode node = parse_node(p, AST_ELSE_STMT);
    pushnode(&p->nodes, node);
    parse_next(p);

    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after $<cyan clear bold:else>$");
    _NODE(p, node).else_st.block = parse_block(p, END, "expected a $<cyan clear bold:end>$ to close else");
    
    return node.id;
}
NodeId parse_elif_statement(Parser* p){
    ASTNode node = parse_node(p, AST_ELIF_STMT);
    pushnode(&p->nodes, node);

    parse_next(p);
    _NODE(p, node).if_st.cond = parse_getValue(p);
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after $<cyan clear bold:elif>$ condition");
    
    _NODE(p, node).if_st.then_block.begin = p->nodes.len;
    while(!(matchType(p, END) || matchType(p, ELIF) || matchType(p, ELSE)))
        _NODE(p, node).if_st.then_block.last = parse_getAST(p);
    _NODE(p, node).if_st.then_block.count = p->nodes.len - _NODE(p, node).if_st.then_block.begin;

    switch(parse_peek(p).type){
        case ELIF: _NODE(p, node).if_st.else_block = parse_elif_statement(p); break;
        case ELSE: _NODE(p, node).if_st.else_block = parse_else_statement(p); break;
        case END: parse_next(p); break;
        default: parse_debug(p, ERROR, "expected at least $<cyan clear bold:end>$, $<cyan clear bold:else>$, or $<cyan clear bold:elif>$ at the end of the $<cyan clear bold:elif>$"); break;
    }
    return node.id;
}
NodeId parse_if_statement(Parser* p){
    ASTNode node = parse_node(p, AST_IF_STMT);
    pushnode(&p->nodes, node);

    parse_next(p);
    _NODE(p, node).if_st.cond = parse_getValue(p);
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after $<cyan clear bold:if>$ condition");
    
    _NODE(p, node).if_st.then_block.begin = p->nodes.len;
    while(!(matchType(p, END) || matchType(p, ELIF) || matchType(p, ELSE)))
        _NODE(p, node).if_st.then_block.last = parse_getAST(p);
    _NODE(p, node).if_st.then_block.count = p->nodes.len - _NODE(p, node).if_st.then_block.begin;
    
    switch(parse_peek(p).type){
        case ELIF: _NODE(p, node).if_st.else_block = parse_elif_statement(p); break;
        case ELSE: _NODE(p, node).if_st.else_block = parse_else_statement(p); break;
        case END: parse_next(p); break;
        default: parse_debug(p, -1, "expected at least $<cyan clear bold:end>$, $<cyan clear bold:else>$, or $<cyan clear bold:elif>$ at the end of the $<cyan clear bold:if>$"); break;
    }
    return node.id;
}


NodeId parse_while_statement(Parser* p){
    ASTNode node = parse_node(p, AST_WHILE_STMT);
    pushnode(&p->nodes, node);

    parse_next(p);
    _NODE(p, node).while_st.cond = parse_getValue(p);
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after the $<cyan clear bold:while>$ condition");
    _NODE(p, node).while_st.block = parse_block(p, END, "expected a $<cyan clear bold:end>$ to close $<cyan clear bold:while>$");
    
    return node.id;
}
NodeId parse_do_while_statement(Parser* p){
    ASTNode node = parse_node(p, AST_DOWHILE_STMT);
    pushnode(&p->nodes, node);

    parse_next(p);
    _NODE(p, node).dowhile_st.block = parse_block(p, WHILE, "expected a $<cyan clear bold:while>$ to close $<cyan clear bold:do-while>$");
    
    _NODE(p, node).dowhile_st.cond = parse_getValue(p);
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after the $<cyan clear bold:do-while>$ condition");

    return node.id;
}


void parse_case_statement(Parser* p){
    ASTNode node = parse_node(p, AST_CASE_STMT);
    pushnode(&p->nodes, node);
    
    parse_next(p);
    _NODE(p, node).case_st.cond = parse_getValue(p);
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after the $<cyan clear bold:case>$ condition");
    
    _NODE(p, node).case_st.body.begin = p->nodes.len;
    while(!(matchType(p, END) || matchType(p, CASE) || matchType(p, DEFAULT)))
        _NODE(p, node).case_st.body.last = parse_getAST(p);
    _NODE(p, node).case_st.body.count = p->nodes.len - _NODE(p, node).case_st.body.begin;
    
    if(!(matchType(p, END) || matchType(p, CASE) || matchType(p, DEFAULT))) 
        parse_debug(p, ERROR, "expected at least $<cyan clear bold:switch end>$, another $<cyan clear bold:case>$, or $<cyan clear bold:default case>$ at end of $<cyan clear bold:case>$");
}
NodeId parse_default_statement(Parser* p){
    ASTNode node = parse_node(p, AST_DEFAULT_STMT);
    pushnode(&p->nodes, node);
    
    parse_next(p);
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after the $<cyan clear bold:default case>$");
    
    _NODE(p, node).case_st.body.begin = p->nodes.len;
    while(!(matchType(p, END) || matchType(p, CASE) || matchType(p, DEFAULT)))
        _NODE(p, node).case_st.body.last = parse_getAST(p);
    _NODE(p, node).case_st.body.count = p->nodes.len - _NODE(p, node).case_st.body.begin;
    
    if(!(matchType(p, END) || matchType(p, CASE))) 
        parse_debug(p, ERROR, "expected at least $<cyan clear bold:switch end>$ or another $<cyan clear bold:case>$ at end of $<cyan clear bold:default case>$");
    return node.id;
}
NodeId parse_switch_statement(Parser* p){
    ASTNode node = parse_node(p, AST_SWITCH_STMT);
    pushnode(&p->nodes, node);

    parse_next(p);
    _NODE(p, node).switch_st.value = parse_expr(p, get_precedence(parse_peek(p).type));
    expectToken(p, PUNCT, COLON, "expected a $<cyan clear bold::>$ after the $<cyan clear bold:switch>$ value");
    _NODE(p, node).switch_st.cases.begin = p->nodes.len;
    _NODE(p, node).switch_st.default_case = 0; // sentinela inicializada
    while(parse_peek(p).type != END && !parse_isEof(p)){
        switch(parse_peek(p).type){
            case CASE: parse_case_statement(p); break;
            case DEFAULT:
                if (_NODE(p, node).switch_st.default_case == 0) {
                    _NODE(p, node).switch_st.default_case = parse_default_statement(p);
                } else {
                    parse_debug(p, ERROR, "$<cyan clear bold:switch>$ already have a $<cyan clear bold:default case>$");
                }
                break;
            default: parse_debug(p, ERROR, "expected a $<cyan clear bold:case>$ in $<cyan clear bold:switch>$"); break;
        }
    }
    expectToken(p, CONTROL, END, "expected a $<cyan clear bold:end>$ at end of $<cyan clear bold:switch>$");
    _NODE(p, node).switch_st.cases.count = p->nodes.len - _NODE(p, node).switch_st.cases.begin;
    
    return node.id;
}
