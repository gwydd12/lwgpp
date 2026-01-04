#ifndef LWGPP_PARSER_H
#define LWGPP_PARSER_H
#include "../token/Token.h"
#include "../ast/Statement.h"

#include <memory>
#include <vector>
#include <deque>

namespace parser {
    /**
     * Abstract base class for parsers.
     *
     * Defines the interface and common functionality for
     * concrete parser implementations.
     */
    class Parser {
    public:
        virtual ~Parser() = default;
        virtual std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) = 0;

    protected:
        std::deque<Token> tokens_;
        int lastLine = 1;

        /**
         *  Must be implemented here to be used by the derived classes.
         */
        template <TokenCategory Category, Category... TokenType>
        Token expectAndConsumeToken() {
            if (isAtEnd()) {
                throw std::runtime_error("Unexpected end of input.");
            }

            //Runtime check to verify that the next token matches one of the expected types.
            if (const Token& currentToken = peek(); !(currentToken.is<Category, TokenType>() || ...)) {
                throw std::runtime_error("Unexpected token type.");
            }

            return consumeToken();
        }

        bool isAtEnd() const;
        const Token& peek() const;
        Token consumeToken();
        void validateSemicolon();
        void setTokens(std::vector<Token> t);
        std::unique_ptr<Assignment> parseAssignment(int line);
    };
}

#endif // LWGPP_PARSER_H