/**
 * @file parser.hpp
 * @author fuechs
 * @brief notc parser
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */

#pragma once

#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    Parser(const Token::Vec &tokens) : tokens(tokens), current(tokens.begin()) {}

    RootAST *parse();

private:
    const Token::Vec &tokens;
    Token::Vec::const_iterator current;

    StmtAST *parseStmt();
    StmtAST *parseFunction();
};