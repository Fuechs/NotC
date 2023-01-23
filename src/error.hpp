/**
 * @file error.hpp
 * @author fuechs
 * @brief notc error
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

class NCError {
public:
    enum Type {
        UNEXPECTED_TOKEN,
        LEAKING_STRING,
        UNKNOWN_ERROR,
    };

    typedef std::vector<NCError> Vec;

    NCError(Type type, size_t line, size_t col, const std::string &fileName, std::string message = "");
    NCError(Type type, Token &token, const std::string &fileName, std::string message = "");
    ~NCError();

    void report();

private:
    Type type;
    size_t line, col;
    const std::string &fileName;
    std::string message;
    bool reported = false;

    const std::vector<std::string> TypeStr = {
        "Unexpected Token",
        "Leaking String",
        "Unknown Error",
    };
};

extern NCError::Vec errors;

void throwErrors();
