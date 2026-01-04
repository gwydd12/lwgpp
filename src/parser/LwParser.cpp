#include "LwParser.h"
#include <algorithm>

using namespace parser::lw;

std::vector<std::unique_ptr<Statement>>
LwParser::parse(std::vector<Token> tokens) {
    setTokens(std::move(tokens));
    encounteredEnd_ = false;
    auto statements = parseLW();
    validateClosingSequence(lastLine);
    return statements;
}

std::vector<std::unique_ptr<Statement>> LwParser::parseLW() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (!tokens_.empty()) {
        const Token& currentToken = peek();
        lastLine = currentToken.line;

        if (currentToken.is<StaticToken>()) {
            switch (currentToken.getType<StaticTokenType>()) {
                case StaticTokenType::LOOP:
                    statements.push_back(parseLoop());
                    break;

                case StaticTokenType::WHILE:
                    statements.push_back(parseWhile());
                    break;

                case StaticTokenType::END:
                    parseEnd();
                    validateSemicolon();
                    encounteredEnd_ = true;
                    break;

                case StaticTokenType::SEMICOLON:
                    consumeToken();
                    break;

                default:
                    throw std::runtime_error(
                        "Unexpected static token at line " +
                        std::to_string(currentToken.line));
            }
        } else if (currentToken.is<DynamicTokenType, DynamicTokenType::VARIABLE>()) {
            statements.push_back(parseAssignment(currentToken.line));
            validateSemicolon();
        } else {
            throw std::runtime_error(
                "Unexpected token at line " +
                std::to_string(currentToken.line));
        }


        if (encounteredEnd_) {
            encounteredEnd_ = false;
            break;
        }
    }

    return statements;
}

std::unique_ptr<Loop> LwParser::parseLoop() {
    balancedIteration_.push_back(expectAndConsumeToken<StaticTokenType, StaticTokenType::LOOP>());

    const Token& conditionToken = expectAndConsumeToken<
        DynamicTokenType,
        DynamicTokenType::VARIABLE,
        DynamicTokenType::CONSTANT>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::DO>();

    auto body = parseLW();
    bool isConstant = conditionToken.is<DynamicTokenType, DynamicTokenType::CONSTANT>();
    return std::make_unique<Loop>(
        isConstant,
        conditionToken.getStringValue(),
        std::move(body),
        conditionToken.line
    );
}

std::unique_ptr<While> LwParser::parseWhile() {
    balancedIteration_.push_back(expectAndConsumeToken<StaticTokenType, StaticTokenType::WHILE>());

    const Token& variableToken = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::VARIABLE>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::GREATER_THAN>();
    const Token& constantToken = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::CONSTANT>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::DO>();

    auto body = parseLW();
    return std::make_unique<While>(
        variableToken.getStringValue(),
        constantToken.getIntValue(),
        std::move(body),
        variableToken.line
    );
}

void LwParser::parseEnd() {
    if (!isBalanced< StaticTokenType, StaticTokenType::LOOP, StaticTokenType::WHILE>()) {
        throw std::runtime_error("Unmatched END statement");
    }

    expectAndConsumeToken<StaticTokenType, StaticTokenType::END>();
    encounteredEnd_ = true;
}

void LwParser::validateClosingSequence(const int line) const {
    if (!balancedIteration_.empty()) {
        throw std::runtime_error(
            "Unmatched END statement at line " +
            std::to_string(line));
    }
}
