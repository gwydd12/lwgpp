#include <iostream>
#include <memory>
#include <vector>

#include "parser/Parser.h"
#include "token/Token.h"

int main() {
    auto parser = std::make_unique<LWParser>();

    // build or populate your tokens vector here
    std::vector const tokens = {
        // Example tokens (you would replace these with actual tokens)
        Token(StaticToken(StaticTokenType::LOOP), 1),
        Token(DynamicToken(DynamicTokenType::VARIABLE, "i"), 1),
        Token(StaticToken(StaticTokenType::DO), 1),
        Token(DynamicToken(DynamicTokenType::VARIABLE, "i"), 2),
        Token(StaticToken(StaticTokenType::EQUALS), 2),
        Token(DynamicToken(DynamicTokenType::VARIABLE, "i"), 2),
        Token(StaticToken(StaticTokenType::PLUS), 2),
        Token(DynamicToken(DynamicTokenType::CONSTANT, "1"), 2),
        Token(StaticToken(StaticTokenType::SEMICOLON), 2),
        Token(StaticToken(StaticTokenType::END), 3)
    };

    const auto stmts = parser->parse(tokens);
    return 0;
}