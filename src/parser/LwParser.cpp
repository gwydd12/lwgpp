#include "LwParser.h"
#include <algorithm>

using namespace lw_parser;
std::vector<std::unique_ptr<Statement>>
LwParser::parse(std::vector<Token> tokens) {
    setTokens(std::move(tokens));
    encounteredEnd = false;
    auto statements = parseLW();
    validateClosingSequence(lastLine);
    return statements;
}

std::vector<std::unique_ptr<Statement>> LwParser::parseLW() {
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
                    parseEnd();
                    validateSemicolon();
                    encounteredEnd = true;
                    return statements;

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
        if (encounteredEnd) {
            encounteredEnd = false;
            break;
        }
    }

    return statements;
}

std::unique_ptr<Loop> LwParser::parseLoop() {
    balancedIteration.push_back(expectAndConsumeToken<StaticTokenType, StaticTokenType::LOOP>());
    const Token& conditionToken = expectAndConsumeToken<
        DynamicTokenType,
        DynamicTokenType::VARIABLE,
        DynamicTokenType::CONSTANT
    >();
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
    balancedIteration.push_back(expectAndConsumeToken<StaticTokenType, StaticTokenType::WHILE>());

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

/**
 * SFINAE - Substitution Failure Is Not An Error
 * We use SFINAE to enable this function only for enum types.
 * The type trait std::is_enum_v checks if TokenCategory is an enum type at compile time.
 * In case you'll pass a non-enum type, the substitution will fail and this function will not be considered during overload resolution.
 */
template<typename TokenCategory, TokenCategory... Expected,
         std::enable_if_t<std::is_enum_v<TokenCategory>, int>>
bool LwParser::isBalanced() {
    if (balancedIteration.empty()) return false;

    if (const Token& token = balancedIteration.back(); (... || token.is<TokenCategory, Expected>())) {
        balancedIteration.pop_back();
        return true;
    }
    return false;
}

void LwParser::parseEnd() {
    if (!isBalanced< StaticTokenType,
            StaticTokenType::LOOP,
            StaticTokenType::WHILE>()) {
        throw std::runtime_error("Unmatched END statement");
    }
    expectAndConsumeToken<StaticTokenType, StaticTokenType::END>();
    encounteredEnd = true;
}

void LwParser::validateClosingSequence(const int line) const {
    if (!balancedIteration.empty()) {
        throw std::runtime_error(
            "Unmatched END statement at line " +
            std::to_string(line));
    }
}
