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
NUMBER = tok()
IDENTIFIER = tok()
LPAREN = tok()
RPAREN = tok()
POINTER = tok()
STRING = tok()
SEMICOLON = tok()
PLUS = tok()
MINUS = tok()
STAR = tok()
SLASH = tok()
EQUAL = tok()
EQUALS = tok()

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
    line = line.split("//")[0]
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
        elif c() == '+':
            tokens.append(Token(PLUS, id, idx + 1, '+'))
            idx += 1
        elif c() == '-':
            if c(1) == '>':
                tokens.append(Token(POINTER, id, idx + 1, "->"))
                idx += 1
            else:
                tokens.append(Token(MINUS, id, idx + 1, '-'))
            idx += 1
        elif c() == '*':
            tokens.append(Token(STAR, id, idx + 1, '*'))
            idx += 1
        elif c() == '/':
            tokens.append(Token(SLASH, id, idx + 1, '/'))
            idx += 1
        elif c() == '=':
            if c(1) == '=':
                tokens.append(Token(EQUALS, id, idx + 1, "=="))
                idx += 1
            else:
                tokens.append(Token(EQUAL, id, idx + 1, "="))
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
        elif c().isdigit():
            col = idx
            lexeme = c()
            idx += 1
            while idx < len(line) and c().isdigit() or c() == '.':
                lexeme += c()
                idx += 1
                if c(-1) == '.':
                    while idx < len(line) and c().isdigit():
                        lexeme += c()
                        idx += 1
            tokens.append(Token(NUMBER, idx, col, lexeme))
        elif c().isspace():
            idx += 1
        else:
            print("UNKNOWN CHARACTER:", c())
            idx += 1
    return tokens

def lex(source: str) -> list[Token]:
    tokens: list[Token] = []
    for id, line in enumerate(source.split("\n")):
        tokens.extend(lex_line(id + 1, line))
    return tokens
        
def read_file(path: str) -> str:
    with open(path, 'r') as file:
        return file.read()
    
from enum import Enum, IntEnum

class Type(Enum):
    VOID = "void"
    INT = "int"
    STR = "str"
    
iop = 0
def get_iop() -> int:
    global iop
    ret = iop
    iop += 1
    return ret
    
class Op(IntEnum):
    CONSTANT = get_iop()
    CONSTANT_LONG = get_iop()
    NULL = get_iop()
    TRUE = get_iop()
    FALSE = get_iop()
    EQUAL = get_iop()
    GREATER = get_iop()
    LESS = get_iop()
    ADD = get_iop()
    SUBTRACT = get_iop()
    MULTIPLY = get_iop()
    DIVIDE = get_iop()
    NOT = get_iop()
    NEGATE = get_iop()
    RETURN = get_iop()
        
@dataclass
class AST:
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        return opcodes, constants

@dataclass
class SymbolExpr(AST):
    symbol: str
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        return opcodes, constants
   
@dataclass 
class BooleanExpr(AST):
    value: bool
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        opcodes.append(Op.TRUE if self.value is True else Op.FALSE)
        return opcodes, constants
    
@dataclass
class NumberExpr(AST):
    value: float
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        opcodes.extend([Op.CONSTANT, len(constants)])
        constants.append(self.value)
        return opcodes, constants

@dataclass
class StringExpr(AST):
    value: str
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        return opcodes, constants

@dataclass
class UnaryExpr(AST):
    op: str
    expr: AST
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        opcodes, generate = self.expr.generate(opcodes, constants)
        match self.op:
            case '-': opcodes.append(Op.NEGATE)
            case '!': opcodes.append(Op.NOT)
        return opcodes, constants

@dataclass
class BinaryExpr(AST):
    op: str
    LHS: AST
    RHS: AST
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        opcodes, constants = self.LHS.generate(opcodes, constants)
        opcodes, constants  = self.RHS.generate(opcodes, constants)
        match self.op:
            case '+': opcodes.append(Op.ADD)
            case '-': opcodes.append(Op.SUBTRACT)
            case '*': opcodes.append(Op.MULTIPLY)
            case '/': opcodes.append(Op.DIVIDE)
            case "==": opcodes.append(Op.EQUAL)
        return opcodes, constants
    
@dataclass
class Variable(AST):
    type: Type
    symbol: str
    value: AST
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        return opcodes, constants
    
@dataclass
class CodeBlock(AST):
    body: list[AST]
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        return opcodes, constants

@dataclass
class Function(AST):
    type: Type
    symbol: str
    parameters: list[Variable]
    body: AST # code block or single statement
    
    def generate(self, opcodes: list[Op], constants: list[float]) -> tuple[list[Op], list[float]]:
        return opcodes, constants
    
@dataclass
class Root(AST):
    program: list[AST]
    
    def generate(self, opcodes: list[Op] = [], constants: list[float] = []) -> tuple[list[Op], list[float]]:
        for stmt in self.program:
            opcodes, constants = stmt.generate(opcodes, constants)
        return opcodes, constants

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
        return self.parseComparsionExpr()
    
    def parseComparsionExpr(self):
        LHS = self.parseAdditiveExpr()
        while self.c() == "==":
            op = self.eat()
            RHS = self.parseAdditiveExpr()
            LHS = BinaryExpr(op, LHS, RHS)
        return LHS
    
    def parseAdditiveExpr(self):
        LHS = self.parseMultiplicativeExpr()
        while self.c() == '+' or self.c() == '-':
            op = self.eat()
            RHS = self.parseMultiplicativeExpr()
            LHS = BinaryExpr(op, LHS, RHS)
        return LHS
    
    def parseMultiplicativeExpr(self):
        LHS = self.parseUnaryExpr()
        while self.c() == '*' or self.c() == '/':
            op = self.eat()
            RHS = self.parseUnaryExpr()
            LHS = BinaryExpr(op, LHS, RHS)
        return LHS
    
    def parseUnaryExpr(self):
        if self.c().value in "-!":
            op = self.eat()
            return UnaryExpr(op, self.parsePrimaryExpr())
        return self.parsePrimaryExpr()
    
    def parsePrimaryExpr(self):
        token = self.eat()
        if token.type == NUMBER:
            return NumberExpr(float(token.value))
        elif token.type == IDENTIFIER:
            if token.value == "true": 
                return BooleanExpr(True)
            if token.value == "false": 
                return BooleanExpr(False)
            return SymbolExpr(token.value)
        elif token.type == STRING:
            return StringExpr(token.value)
        elif token.type == LPAREN:
            sub_expr = self.parseExpr()
            self.eat(RPAREN)
            return sub_expr
        return None
    
tokens = lex(read_file("main.nc"))
parser = Parser(tokens)
root = parser.parse()
del parser

from pprint import pprint, pformat
pprint(tokens)

def debug(ast: Root) -> None:
    for id, stmt in enumerate(ast.program):
        print(id, '|', end=" ")
        pprint(stmt)

debug(root)
opcodes, constants = root.generate()
opcodes.append(Op.RETURN)
pprint(opcodes)
pprint(constants)

def get_bytes(constants: list[float]) -> bytearray:
    from struct import pack
    _bytes = []
    for constant in constants:
        _bytes.extend(pack("f", constant))
    return bytearray(_bytes)

with open("main.nco", "wb") as out:
    out.write(get_bytes(constants))
    out.write(bytearray([0xc0, 0xff, 0xee]))
    out.write(bytearray(opcodes))
    
with open("main.nco", "rb") as file:
    from struct import unpack
    src = file.read()
    current = None
    i = 0
    while src[i] != 0xc0 and src[i+1] != 0xff and src[i+2] != 0xee:
        # print(hex(src[i]))
        i += 4
    i += 3
    while i < len(src):
        if src[i] == Op.CONSTANT:
            i += 1
            print("CONST", src[i])
        elif src[i] == Op.TRUE:
            print("TRUE")
        elif src[i] == Op.FALSE:
            print("FALSE")
        elif src[i] == Op.NEGATE:
            print("NEGATE")
        elif src[i] == Op.ADD:
            print("ADD")
        elif src[i] == Op.SUBTRACT:
            print("SUBTRACT")
        elif src[i] == Op.MULTIPLY:
            print("MULTIPLY")
        elif src[i] == Op.DIVIDE:
            print("DIVIDE")
        elif src[i] == Op.EQUAL:
            print("EQUAL")
        elif src[i] == Op.RETURN:
            print("RETURN")
        else:
            print(hex(src[i]))
        i += 1