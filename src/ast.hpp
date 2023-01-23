/**
 * @file ast.hpp
 * @author fuechs
 * @brief notc ast
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */

#pragma once

#include <vector>
#include <string>

#include "token.hpp"

class StmtAST {
public:
    typedef std::vector<StmtAST *> Vec;

    virtual ~StmtAST() = 0;
    virtual void debugPrint() = 0;
};

class CodeBlockAST : public StmtAST {
    StmtAST::Vec body;
public:
    CodeBlockAST(StmtAST::Vec body = {}) : body(body) {}
    ~CodeBlockAST();

    void debugPrint();
};

class FunctionAST : public StmtAST {
    Token::Type type;
    std::string name;
    StmtAST::Vec arguments;
    CodeBlockAST *body;

public:
    FunctionAST(Token::Type type, std::string name, StmtAST::Vec arguments, CodeBlockAST *body) 
    : type(type), name(name), arguments(arguments), body(body) {}
    ~FunctionAST();

    void debugPrint();

};

class RootAST : public StmtAST {
    StmtAST::Vec program;
public:
    RootAST() : program({}) {}
    ~RootAST();

    void debugPrint();
};