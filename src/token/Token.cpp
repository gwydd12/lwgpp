//
// Created by stdin on 10/30/25.
//

#include "TokenType.h"

class Token {
public:
    TokenType _type;
    int line;
    int value;

    Token(TokenType type, int line, int value);
    Token(TokenType type, int line);
};