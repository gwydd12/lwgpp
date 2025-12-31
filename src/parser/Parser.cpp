#include "Parser.h"
#include <stdexcept>

using namespace std;

bool Parser::isAtEnd() const {
    return tokens.empty();
}

const Token& Parser::peek() const {
    if (isAtEnd()) {
        throw runtime_error("Unexpected end of input.");
    }
    return tokens.front();
}

Token Parser::consumeToken() {
    if (isAtEnd()) {
        throw runtime_error("Unexpected end of input.");
    }

    Token tok = tokens.front();
    tokens.pop_front();
    lastLine = tok.line;
    return tok;
}



void Parser::skipToNextLine() {
    if (isAtEnd()) return;

    int line = peek().line;
    while (!isAtEnd() && peek().line == line) {
        consumeToken();
    }
}

void Parser::validateSemicolon() {
    if (isAtEnd()) return;

    if (!peek().is<StaticTokenType, StaticTokenType::SEMICOLON>()) {
        throw runtime_error(
            "Expected semicolon at line " +
            to_string(peek().line));
    }

    consumeToken();
}

void Parser::setTokens(vector<Token> t) {
    tokens = deque(t.begin(), t.end());
    lastLine = 1;
}

unique_ptr<Assignment> Parser::parseAssignment(int line) {
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