/**
 * @file lexer.cpp
 * @author fuechs
 * @brief notc lexer
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023, Fuechs. All rights reserved.
 * 
 */

#include <sstream>
#include "lexer.hpp"

using std::string, std::to_string, std::stringstream;

Token::Token(Type type, string value, size_t line, size_t col)
: type(type), value(value), line(line), col(col) {}

Token::~Token() { value.clear(); }

string Token::str() {
    stringstream ss;
    ss << "<Line " << line << ":" << col << " \"" << value <<"\">";
    return ss.str();
}

Lexer::Lexer(const string &fileName, const string &source) 
: fileName(fileName), source(source), tokens(Token::Vec()), line(1), col(1), curTok(Token()) {}

Lexer::~Lexer() { tokens.clear(); }

Token::Vec Lexer::lex() { 
    current = source.begin();
    while (current != source.end()) {
        curTok = Token();
        curTok.line = line;
        curTok.col = col;
        switch (*current) {
            case '"':
                curTok.type = Token::STRING;
                advance();
                while (*current != '"') {
                    if (*current == '\n') {
                        errors.push_back(NCError(NCError::LEAKING_STRING, curTok, fileName, "expected a '\"' before end of line"));
                        advance();
                        continue;
                    }
                    curTok.value.push_back(*current);
                    advance();
                }
                advance();
                break;
            
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                curTok.type = Token::NUMBER;
                while (isdigit(*current)) {
                    curTok.value.push_back(*current);
                    advance();
                }
                break;

            case ' ':
            case '\t':
            case '\n':
                advance();
                continue;

            case '(':
                curTok.type = Token::LPAREN;
                curTok.value = "(";
                advance();
                break;
            
            case ')':
                curTok.type = Token::RPAREN;
                curTok.value = ")";
                advance();
                break;

            case '{':
                curTok.type = Token::LBRACE;
                curTok.value = "{";
                advance();
                break;
            
            case '}':
                curTok.type = Token::RBRACE;
                curTok.value = "}";
                advance();
                break;

            case '-':
                if (peek() == '>') {
                    curTok.type = Token::POINTER;
                    curTok.value = "->";
                    advance(2);
                    break;
                }

            case ';': 
                curTok.type = Token::SEMICOLON;
                curTok.value = ";";
                advance();
                break;

            default:
                if (isalpha(*current) || *current == '_') {
                    while (isalpha(*current) || isdigit(*current) || *current == '_') {
                        curTok.value.push_back(*current);
                        advance();
                    }

                    if      (curTok.value == "def")     curTok.type = Token::KEY_DEF;
                    else if (curTok.value == "void")    curTok.type = Token::KEY_VOID;
                    else if (curTok.value == "int")     curTok.type = Token::KEY_INT;
                    else if (curTok.value == "float")   curTok.type = Token::KEY_FLOAT;
                    else if (curTok.value == "str")     curTok.type = Token::KEY_STR;
                    else if (curTok.value == "ret")     curTok.type = Token::KEY_RET;
                    else                                curTok.type = Token::IDENTIFIER;

                    break;
                }

                stringstream ss;
                ss << "'" << *current << "'";
                errors.push_back(NCError(NCError::UNEXPECTED_TOKEN, curTok, fileName, ss.str()));
                advance();
                continue;
        }

        tokens.push_back(curTok);
    }

    return tokens; 
}

void Lexer::advance(size_t steps) {
    while (steps --> 0 && current != source.end()) {
        if (*current++ == '\n') {
            ++line;
            col = 1;
        } else
            ++col;
    }   
}

char Lexer::peek(int steps) { return *(current + steps); }