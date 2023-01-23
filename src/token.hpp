/**
 * @file token.hpp
 * @author fuechs
 * @brief notc token
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>

class Token {
public:
    enum Type {
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        POINTER,
        COLON,
        SEMICOLON,
        IDENTIFIER,
        NUMBER,
        FLOAT,
        STRING,

        KEY_DEF,
        KEY_VOID,
        KEY_INT,
        KEY_FLOAT,
        KEY_STR,
        KEY_RET,

        NONE,
    };

    typedef std::vector<Token> Vec;

    Token(Type type = NONE, std::string value = "", size_t line = 1, size_t col = 1);
    ~Token();

    std::string str();

    Type type;
    std::string value;
    size_t line;
    size_t col;
};

const std::vector<std::string> TokenTypeStr = {
    "(", ")", "{", "}", "->", ":", ";", 
    "identifier", "0", "0.0", "\"string\"",
    "def", "void", "int", "float", "str", "ret"
};

std::ostream &operator<<(std::ostream &os, Token::Type type) {
    return os << TokenTypeStr[type];
}