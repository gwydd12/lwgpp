#include "Parser.h"
#include <algorithm>
#include <stdexcept>
#include <deque>

using namespace std;

Token Parser::peek() {
    if (isAtEnd()) {
        throw std::runtime_error("Unexpected end of input.");
    }
    return tokens[current];
}

Token& Parser::expectDynamic(DynamicTokenType expectedType) {
    if (isAtEnd() || !std::holds_alternative<DynamicToken>(tokens[current].value)) {
        throw std::runtime_error("Expected dynamic token");
    }
    if (std::get<DynamicToken>(tokens[current].value).type != expectedType) {
        throw std::runtime_error("Expected dynamic token type mismatch");
    }
    return tokens[current++];
}

Token& Parser::expectStatic(StaticTokenType expectedType) {
    if (isAtEnd() || !std::holds_alternative<StaticToken>(tokens[current].value)) {
        throw std::runtime_error("Expected static token");
    }
    if (std::get<StaticToken>(tokens[current].value).type != expectedType) {
        throw std::runtime_error("Unexpected static token type");
    }
    return tokens[current++];
}

Token& Parser::expectOneOfStatic(std::initializer_list<StaticTokenType> types) {
    if (isAtEnd() || !std::holds_alternative<StaticToken>(tokens[current].value)) {
        throw std::runtime_error("Expected static token");
    }
    const auto st = std::get<StaticToken>(tokens[current].value).type;
    if (std::find(types.begin(), types.end(), st) == types.end()) {
        throw std::runtime_error("Unexpected operator");
    }
    return tokens[current++];
}

Token& Parser::expectOneOfDynamic(std::initializer_list<DynamicTokenType> types) {
    if (isAtEnd() || !std::holds_alternative<DynamicToken>(tokens[current].value)) {
        throw std::runtime_error("Expected dynamic token");
    }
    const auto dt = std::get<DynamicToken>(tokens[current].value).type;
    if (std::find(types.begin(), types.end(), dt) == types.end()) {
        throw std::runtime_error("Unexpected dynamic token type");
    }
    return tokens[current++];
}

bool Parser::isAtEnd() const {
    return current >= static_cast<int>(tokens.size());
}

void Parser::skipToNextLine() {
    if (isAtEnd()) return;
    int line = tokens[current].line;
    while (!isAtEnd() && tokens[current].line == line) {
        ++current;
    }
}

void Parser::validateSemicolon() {
    if (isAtEnd() || !tokens[current].isStatic() || std::get<StaticToken>(tokens[current].value).type != StaticTokenType::SEMICOLON) {
        throw std::runtime_error("Expected semicolon");
    }
    ++current;
}

void Parser::setTokens(std::vector<Token> t) {
    tokens = std::move(t);
    current = 0;
    lastLine = 1;
}

std::unique_ptr<Assignment> Parser::parseAssignment(int line) {
    Token& assigneeVariable = expectDynamic(DynamicTokenType::VARIABLE);
    expectStatic(StaticTokenType::EQUALS);
    Token& assignVariable = expectDynamic(DynamicTokenType::VARIABLE);
    Token& op = expectOneOfStatic({StaticTokenType::PLUS, StaticTokenType::MINUS});
    Token& constant = expectDynamic(DynamicTokenType::CONSTANT);

    return std::make_unique<Assignment>(
        assigneeVariable.getStringValue(),
        assignVariable.getStringValue(),
        getOperator(op),
        constant.getIntValue(),
        line
    );
}

std::vector<std::unique_ptr<Statement>> LWParser::parse(std::vector<Token> tokensVec) {
    setTokens(std::move(tokensVec));
    return parseLW(tokens);
}

std::vector<std::unique_ptr<Statement>> LWParser::parseLW(std::vector<Token>& /*unused_tokens_ref*/) {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        Token currentToken = peek();
        lastLine = currentToken.line;

        if (currentToken.isStatic()) {
            switch (currentToken.getStatic().type) {
                case StaticTokenType::LOOP:
                    statements.push_back(parseLoop());
                    break;
                case StaticTokenType::WHILE:
                    statements.push_back(parseWhile());
                    break;
                case StaticTokenType::END:
                    parseEnd();
                    break;
                case StaticTokenType::SEMICOLON:
                    expectStatic(StaticTokenType::SEMICOLON);
                    break;
                default:
                    throw std::runtime_error("Unexpected static token at line " + std::to_string(currentToken.line));
            }
        } else if (currentToken.isDynamic()) {
            statements.push_back(parseAssignment(currentToken.line));
        } else {
            throw std::runtime_error("Unexpected token at line " + std::to_string(currentToken.line));
        }
    }

    if (!balancedIteration.empty() && !encounteredEnd) {
        throw std::runtime_error("Missing END statement for opened LOOP/WHILE");
    }

    return statements;
}

std::unique_ptr<Loop> LWParser::parseLoop() {
    balancedIteration.push_back(StaticTokenType::LOOP);
    expectStatic(StaticTokenType::LOOP);
    Token& conditionToken = expectOneOfDynamic({DynamicTokenType::VARIABLE, DynamicTokenType::CONSTANT});
    expectStatic(StaticTokenType::DO);

    // parse body until matching END; reuse parseLW on the same token stream
    return std::make_unique<Loop>(
        conditionToken.getDynamic().type == DynamicTokenType::CONSTANT,
        conditionToken.getDynamic().value,
        parseLW(tokens),
        conditionToken.line
    );
}

std::unique_ptr<While> LWParser::parseWhile() {
    balancedIteration.push_back(StaticTokenType::WHILE);
    expectStatic(StaticTokenType::WHILE);
    Token& variableToken = expectDynamic(DynamicTokenType::VARIABLE);
    expectStatic(StaticTokenType::GREATER_THAN);
    Token& constantToken = expectDynamic(DynamicTokenType::CONSTANT);
    expectStatic(StaticTokenType::DO);

    return std::make_unique<While>(
        variableToken.getDynamic().value,
        std::stoi(constantToken.getDynamic().value),
        parseLW(tokens),
        variableToken.line
    );
}

void LWParser::parseEnd() {
    if (!isBalancedStatementSequence({StaticTokenType::LOOP, StaticTokenType::WHILE})) {
        throw std::runtime_error("Unmatched END statement");
    }
    expectStatic(StaticTokenType::END);
    encounteredEnd = true;
}

void LWParser::validateClosingSequence(int line) {
    if (!isBalancedStatementSequence({StaticTokenType::LOOP, StaticTokenType::WHILE})) {
        throw std::runtime_error("Unmatched END statement at line " + std::to_string(line));
    }
}

bool LWParser::isBalancedStatementSequence(std::initializer_list<StaticTokenType> expectedTypes) {
    bool found = false;
    for (const auto type : expectedTypes) {
        if (!balancedIteration.empty() && balancedIteration.back() == type) {
            balancedIteration.pop_back();
            found = true;
            break;
        }
    }
    return found;
}

/* ----- GOTOParser stubs (implement full logic in Parser.h/.cpp as needed) ----- */

std::vector<std::unique_ptr<Statement>> GOTOParser::parse(std::vector<Token> /*tokens*/) {
    throw std::runtime_error("GOTOParser::parse not implemented");
}

std::unique_ptr<Halt> GOTOParser::parseHalt(int /*line*/, int /*markerLine*/) {
    throw std::runtime_error("GOTOParser::parseHalt not implemented");
}

std::unique_ptr<If> GOTOParser::parseIf(int /*line*/, int /*markerLine*/) {
    throw std::runtime_error("GOTOParser::parseIf not implemented");
}

std::unique_ptr<Goto> GOTOParser::parseGotoStatement(int /*line*/, int /*markerLine*/) {
    throw std::runtime_error("GOTOParser::parseGotoStatement not implemented");
}

void GOTOParser::checkGotoValues() {
    throw std::runtime_error("GOTOParser::checkGotoValues not implemented");
}
