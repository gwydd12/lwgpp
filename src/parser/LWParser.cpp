#include "LWParser.h"

#include <algorithm>

std::vector<std::unique_ptr<Statement>>
LWParser::parse(std::vector<Token> tokensVec) {
    setTokens(std::move(tokensVec));
    return parseLW();
}

std::vector<std::unique_ptr<Statement>> LWParser::parseLW() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (!isAtEnd()) {
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
                    //parseEnd();
                    //validateSemicolon();
                    return statements;

                case StaticTokenType::SEMICOLON:
                    consumeToken();
                    break;

                default:
                    throw std::runtime_error(
                        "Unexpected static token at line " +
                        std::to_string(currentToken.line));
            }
        } else if (currentToken.is<DynamicToken>()) {
            statements.push_back(
                parseAssignment(currentToken.line));
        } else {
            throw std::runtime_error(
                "Unexpected token at line " +
                std::to_string(currentToken.line));
        }
    }

    return statements;
}

std::unique_ptr<Loop> LWParser::parseLoop() {
    balancedIteration.push_back(StaticTokenType::LOOP);
    expectAndConsumeToken<StaticTokenType, StaticTokenType::LOOP>();
    const Token& conditionToken = expectAndConsumeToken<
        DynamicTokenType,
        DynamicTokenType::VARIABLE,
        DynamicTokenType::CONSTANT
    >();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::DO>();
    auto body = parseLW();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::END>();
    validateSemicolon();

    bool isConstant = conditionToken.is<DynamicTokenType, DynamicTokenType::CONSTANT>();
    return std::make_unique<Loop>(
        isConstant,
        conditionToken.getStringValue(),
        std::move(body),
        conditionToken.line
    );
}

std::unique_ptr<While> LWParser::parseWhile() {
    balancedIteration.push_back(StaticTokenType::WHILE);

    expectAndConsumeToken<StaticTokenType, StaticTokenType::WHILE>();
    const Token& variableToken = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::VARIABLE>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::GREATER_THAN>();
    const Token& constantToken = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::CONSTANT>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::DO>();
    auto body = parseLW();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::END>();
    validateSemicolon();

    return std::make_unique<While>(
        variableToken.getStringValue(),
        constantToken.getIntValue(),
        std::move(body),
        variableToken.line
    );
}

bool LWParser::isBalancedStatementSequence(
    std::initializer_list<StaticTokenType> expectedTypes
) {
    if (balancedIteration.empty()) {
        return false;
    }

    StaticTokenType lastOpened = balancedIteration.back();
    if (std::find(expectedTypes.begin(), expectedTypes.end(), lastOpened) != expectedTypes.end()) {
        balancedIteration.pop_back();
        return true;
    }

    return false;
}

void LWParser::parseEnd() {
    if (!isBalancedStatementSequence(
            {StaticTokenType::LOOP,
             StaticTokenType::WHILE})) {
        throw std::runtime_error("Unmatched END statement");
    }
    expectAndConsumeToken<StaticTokenType, StaticTokenType::END>();
}

void LWParser::validateClosingSequence(const int line) {
    if (!isBalancedStatementSequence(
            {StaticTokenType::LOOP,
             StaticTokenType::WHILE})) {
        throw std::runtime_error(
            "Unmatched END statement at line " +
            std::to_string(line));
    }
}
