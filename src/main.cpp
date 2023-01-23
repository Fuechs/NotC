/**
 * @file main.cpp
 * @author fuechs
 * @brief notc main
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */


#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer.hpp"

std::string readFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "could not open file '" << filePath << "'\n";
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char **argv) { 
    const std::string filePath = "src/examples/main.nc";
    const std::string source = readFile(filePath);
    Lexer lexer = Lexer(filePath, source);
    auto tokens = lexer.lex();

    for (Token &token : tokens)
        std::cout << token.str() << "\n";
    
    throwErrors();

    return 0;
}
