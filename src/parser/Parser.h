#ifndef LWGPP_PARSER_H
#define LWGPP_PARSER_H

#include "../token/Token.h"
#include "../ast/Statement.h"

#include <memory>
#include <vector>
#include <deque>
/**
 * Abstract base class for parsers.
 *
 * Defines the interface and common functionality for
 * concrete parser implementations.
 */
class Parser {
protected:
    std::deque<Token> tokens;
    int lastLine = 1;

    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token& peek() const;
    Token consumeToken();

    template <typename TokenCategory, TokenCategory... TokenType>
    Token expectAndConsumeToken() {
        if (isAtEnd()) {
            throw std::runtime_error("Unexpected end of input.");
        }

        //Runtime check to verify that the next token matches one of the expected types.
        if (const Token& currentToken = peek(); !(currentToken.is<TokenCategory, TokenType>() || ...)) {
            throw std::runtime_error("Unexpected token type.");
        }

        return consumeToken();
    }

    void skipToNextLine();
    void validateSemicolon();
    void setTokens(std::vector<Token> t);

    std::unique_ptr<Assignment> parseAssignment(int line);

public:
    virtual ~Parser() = default;
    virtual std::vector<std::unique_ptr<Statement>>
    parse(std::vector<Token> tokens) = 0;
};



#endif // LWGPP_PARSER_H