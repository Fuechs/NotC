# NotC Parser
# Copyright (c) 2023, Fuechs. All rights reserved.

from dataclasses import dataclass
    
tok_count = -1
def tok() -> int:
    global tok_count
    ret = tok_count
    tok_count += 1
    return ret

NONE = tok()
IDENTIFIER = tok()
LPAREN = tok()
RPAREN = tok()
POINTER = tok()
STRING = tok()
SEMICOLON = tok()

@dataclass
class Token:
    type: int = NONE
    line: int = 1
    col: int = 1
    value: str = ""

    def __eq__(self, value: str) -> bool:
        return self.value == value
    
    def __ne__(self, value: str) -> bool:
        return self.value != value
    
# TODO:
def lex_line(id: int, line: str) -> list[Token]:    
    tokens: list[Token] = []
    idx = 0
    
    def c(peek: int = 0) -> str:
        nonlocal line, idx
        if idx + peek >= len(line):
            return None
        return line[idx + peek]
    
    while idx < len(line):
        if c() == '(':
            tokens.append(Token(LPAREN, id, idx + 1, '('))
            idx += 1
        elif c() == ')':
            tokens.append(Token(RPAREN, id, idx + 1, ')'))
            idx += 1
        elif c() == ';':
            tokens.append(Token(SEMICOLON, id, idx + 1, ';'))
            idx += 1
        elif c() == '-':
            if c(1) == '>':
                tokens.append(Token(POINTER, id, idx + 1, "->"))
                idx += 1
            else:
                pass
            idx += 1
        elif c() == '"':
            col = idx
            lexeme = ""
            idx += 1
            while idx < len(line) and c() != '"':
                lexeme += c()
                idx += 1
            if idx >= len(line):
                print("ERROR: expected double quote at line", id, ":", col)
            idx += 1
            tokens.append(Token(STRING, id, col, lexeme))
        elif c().isalpha() or c() == '_':
            col = idx
            lexeme = c()
            idx += 1
            while idx < len(line) and (c().isalnum() or c() == '_'):
                lexeme += c()
                idx += 1
            tokens.append(Token(IDENTIFIER, id, col, lexeme))
        elif c().isspace():
            idx += 1
        else:
            print("UNKNOWN CHARACTER:", c())
    return tokens

def lex(source: str) -> list[Token]:
    tokens: list[Token] = []
    for id, line in enumerate(source.split("\n")):
        tokens.extend(lex_line(id + 1, line))
    return tokens
        
def read_file(path: str) -> str:
    with open(path, 'r') as file:
        return file.read()
    
from enum import Enum

class Type(Enum):
    VOID = "void"
    INT = "int"
    STR = "str"
        
@dataclass
class AST:
    pass

@dataclass
class SymbolExpr(AST):
    symbol: str

@dataclass
class StringExpr(AST):
    value: str

@dataclass
class UnaryExpr(AST):
    op: str
    expr: AST

@dataclass
class BinaryExpr(AST):
    op: str
    LHS: AST
    RHS: AST
    
@dataclass
class Variable(AST):
    type: Type
    symbol: str
    value: AST
    
@dataclass
class CodeBlock(AST):
    body: list[AST]

@dataclass
class Function(AST):
    type: Type
    symbol: str
    parameters: list[Variable]
    body: AST # code block or single statement
    
@dataclass
class Root(AST):
    program: list[AST]

class Parser:
    def __init__(self, tokens: list[Token]) -> None:
        self.tokens = tokens
        self.idx = 0
        self.ast = Root([])
    
    def c(self, peek: int = 0) -> Token:
        if self.idx + peek >= len(self.tokens):
            return None
        return self.tokens[self.idx]
    
    def eat(self, type: int = NONE) -> Token:
        if type != NONE and (not self.c() or self.c().type != type):
            print("UNEXPECTED TOKEN:", self.c())
        ret = self.c()
        self.idx += 1 
        return ret
    
    def parse(self) -> Root:
        while self.idx < len(tokens):
            self.ast.program.append(self.parseStmt())
        return self.ast
    
    def parseStmt(self):
        stmt = self.parseFunction()
        if stmt and not isinstance(stmt, Function):
            self.eat(SEMICOLON)
        return stmt
    
    def parseFunction(self):
        if self.c() != "def":
            return self.parseExpr()
        self.eat() # def
        symbol = self.eat(IDENTIFIER).value
        self.eat(LPAREN) # TODO: parse parameters, variables
        self.eat(RPAREN)
        self.eat(POINTER)
        type = Type(self.eat().value)
        body = self.parseStmt()
        return Function(type, symbol, [], body)
    
    def parseExpr(self):
        return self.parsePrimaryExpr()
    
    def parsePrimaryExpr(self):
        token = self.eat()
        if token.type == IDENTIFIER:
            return SymbolExpr(token.value)
        elif token.type == STRING:
            return StringExpr(token.value)
        return None
    
tokens = lex(read_file("main.nc"))
parser = Parser(tokens)
root = parser.parse()
del parser

def debug(ast: Root) -> None:
    from pprint import pprint, pformat
    for id, stmt in enumerate(ast.program):
        print(id, '|', end=" ")
        if isinstance(stmt, Function):
            print(stmt.type, stmt.symbol, "(", pformat(stmt.parameters), ")\n  |", end=" ")
            pprint(stmt.body)
            
class Op(Enum):
    LABEL = 5

def generate(ast: Root, path: str) -> None:
    with open(path, "w") as file:
        for stmt in ast.program:
            break

debug(root)
generate(root, "main.nco")