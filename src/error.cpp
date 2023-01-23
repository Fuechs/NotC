/**
 * @file error.cpp
 * @author fuechs
 * @brief notc error
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */


#include <iostream>

#include "error.hpp"

using std::string, std::vector, std::cerr;

NCError::NCError(Type type, size_t line, size_t col, const string &fileName, string message)
: type(type), line(line), col(col), fileName(fileName), message(message) {}

NCError::NCError(Type type, Token &token, const string &fileName, string message)
: type(type), line(token.line), col(token.col), fileName(fileName), message(message) {}

NCError::~NCError() { message.clear(); }

void NCError::report() {
    if (reported)
        return;

    reported = true;
    cerr << fileName << ":" << line << ":" << col << ": " << TypeStr[type] << ": " << message << "\n";
}

NCError::Vec errors = NCError::Vec();

void throwErrors() {
    for (NCError &e : errors)
        e.report();
}