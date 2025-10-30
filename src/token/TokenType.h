//
// Created by stdin on 10/30/25.
//

#ifndef LWGPP_TOKENTYPE_H
#define LWGPP_TOKENTYPE_H


enum TokenType {
    // static token types
    LOOP,
    WHILE,
    DO,
    END,
    EQUALS,
    PLUS,
    MINUS,
    GREATER_THAN,
    IF,
    THEN,
    GOTO,
    HALT,
    SEMICOLON,
    COLON,
    // dynamic token types
    VARIABLE,
    CONSTANT,
    MARKER
};


#endif //LWGPP_TOKENTYPE_H