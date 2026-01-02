#include "Parser.h"
#include <stdexcept>

using namespace parser;

bool Parser::isAtEnd() const {
    return tokens_.empty();
}

const Token& Parser::peek() const {
    if (isAtEnd()) {
        throw std::runtime_error("Unexpected end of input.");
    }
    return tokens_.front();
}

Token Parser::consumeToken() {
    if (isAtEnd()) {
        throw std::runtime_error("Unexpected end of input.");
    }

    Token tok = tokens_.front();
    tokens_.pop_front();
    lastLine = tok.line;
    return tok;
}

void Parser::skipToNextLine() {
    if (isAtEnd()) return;

    const int line = peek().line;
    while (!isAtEnd() && peek().line == line) {
        consumeToken();
    }
}

void Parser::validateSemicolon() {
    if (isAtEnd()) return;

    if (!peek().is<StaticTokenType, StaticTokenType::SEMICOLON>()) {
        throw std::runtime_error(
            "Expected semicolon at line " +
            std::to_string(peek().line));
    }

    consumeToken();
}

void Parser::setTokens(std::vector<Token> t) {
    tokens_ = std::deque(t.begin(), t.end());
    lastLine = 1;
}

std::unique_ptr<Assignment> Parser::parseAssignment(int line) {
    const Token& assigneeVariable = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::VARIABLE>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::EQUALS>();
    const Token& assignVariable = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::VARIABLE>();
    const Token& op = expectAndConsumeToken<StaticTokenType, StaticTokenType::PLUS, StaticTokenType::MINUS>();
    const Token& constant = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::CONSTANT>();

    return make_unique<Assignment>(
        assigneeVariable.getStringValue(),
        assignVariable.getStringValue(),
        getOperator(op),
        constant.getIntValue(),
        line
    );
}