
#ifndef THEIA_AST_HPP 
#define THEIA_AST_HPP

#include "tokens.h"

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : Expr {
    Token value;
};

struct StringExpr : Expr {
    Token value;
};

struct BinaryExpr : Expr {
    Token op;
    Expr* left;
    Expr* right;
};

struct VarExpr : Expr {
    Token name;
};

#endif