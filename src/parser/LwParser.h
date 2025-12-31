#ifndef LWGPP_LWPARSER_H
#define LWGPP_LWPARSER_H
#include "Parser.h"

class LwParser final : public Parser {
public:
    std::vector<std::unique_ptr<Statement>>
    parse(std::vector<Token> tokens) override;

private:
    std::deque<Token> balancedIteration;
    bool encounteredEnd = false;

    std::vector<std::unique_ptr<Statement>> parseLW();
    std::unique_ptr<Loop> parseLoop();
    std::unique_ptr<While> parseWhile();
    void parseEnd();
    void validateClosingSequence(int line) const;

    template<typename TokenCategory, TokenCategory... Expected>
    bool isBalanced();
};

#endif //LWGPP_LWPARSER_H