/**
 * @file ast.cpp
 * @author fuechs
 * @brief notc ast
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */

#include "ast.hpp"

#include <iostream>
using std::cout;

CodeBlockAST::~CodeBlockAST() {
    for (StmtAST *&stmt : body)
        delete stmt;
    body.clear();
}

void CodeBlockAST::debugPrint() {
    cout << "{\n";
    for (StmtAST *&stmt : body) {
        cout << "\t";
        stmt->debugPrint();
        cout << ";\n";
    }
    cout << "}";
}

FunctionAST::~FunctionAST() {
    name.clear();
    for (StmtAST *&stmt : arguments)
        delete stmt;
    delete body;
}

void FunctionAST::debugPrint() {
    cout << "def " << name << "(";
    for (StmtAST *&stmt : arguments) {
        stmt->debugPrint();
        cout << ", ";
    }
    cout << ") -> " << type << " ";
    body->debugPrint();
}

RootAST::~RootAST() {
    for (StmtAST *&stmt : program)
        delete stmt;
}

void RootAST::debugPrint() {
    for (StmtAST *&stmt : program) {
        stmt->debugPrint();
        cout << ";\n";
    }
}