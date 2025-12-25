#ifndef LWGPP_TOKEN_H
#define LWGPP_TOKEN_H

#include <string>
#include "TokenType.h"

class Token {
public:
    TokenType _type;
    int line;
    int value;

    Token(TokenType type, int line, int value);
    Token(TokenType type, int line);

    std::string toString() const;
};

#endif
