/**
 * @file lexer.hpp
 * @author fuechs
 * @brief notc lexer
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */

#pragma once

#include <string>
#include <vector>

#include "token.hpp"
#include "error.hpp"

class Lexer {
public:
    Lexer(const std::string &fileName, const std::string &source);
    ~Lexer();

    Token::Vec lex();

private:
    const std::string &fileName;
    const std::string &source;
    std::string::const_iterator current;
    Token::Vec tokens;
    size_t line, col;
    Token curTok;

    void advance(size_t steps = 1);
    char peek(int steps = 1);
};