#include "Token.h"
#include <sstream>

Token::Token(TokenType type, int line, int value)
    : _type(type), line(line), value(value) {}

Token::Token(TokenType type, int line)
    : _type(type), line(line), value(0) {}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token(" << _type
        << ", line=" << line
        << ", value=" << value << ")";
    return oss.str();
}
