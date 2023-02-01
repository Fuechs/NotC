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

@dataclass
class Token:
    type: int = NONE
    line: int = 1
    col: int = 1
    value: str = ""
    
# TODO:
def lex_line(id: int, line: str) -> list[Token]:    
    tokens: list[Token] = []
    idx = 0
    while idx < len(line):
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
    
print(lex(read_file("main.nc")))