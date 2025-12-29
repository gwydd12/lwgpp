#include "Parser.h"
#include <algorithm>
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

/* =======================
 * expect*
 * ======================= */

Token Parser::expectDynamic(DynamicTokenType expectedType) {
    if (isAtEnd() || !peek().isDynamic()) {
        throw runtime_error("Expected dynamic token");
    }

    if (peek().getDynamic().type != expectedType) {
        throw runtime_error("Expected dynamic token type mismatch");
    }

    return consumeToken();
}

Token Parser::expectStatic(StaticTokenType expectedType) {
    if (isAtEnd() || !peek().isStatic()) {
        throw runtime_error("Expected static token");
    }

    if (peek().getStatic().type != expectedType) {
        throw runtime_error("Unexpected static token type");
    }

    return consumeToken();
}

Token Parser::expectOneOfStatic(
    initializer_list<StaticTokenType> types) {
    if (isAtEnd() || !peek().isStatic()) {
        throw runtime_error("Expected static token");
    }

    const auto st = peek().getStatic().type;
    if (find(types.begin(), types.end(), st) == types.end()) {
        throw runtime_error("Unexpected operator");
    }

    return consumeToken();
}

Token Parser::expectOneOfDynamic(
    initializer_list<DynamicTokenType> types) {
    if (isAtEnd() || !peek().isDynamic()) {
        throw runtime_error("Expected dynamic token");
    }

    const auto dt = peek().getDynamic().type;
    if (find(types.begin(), types.end(), dt) == types.end()) {
        throw runtime_error("Unexpected dynamic token type");
    }

    return consumeToken();
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

    if (peek().isStatic() &&
        peek().getStatic().type == StaticTokenType::SEMICOLON) {
        consumeToken();
    } else {
        throw runtime_error(
            "Expected semicolon at line " +
            to_string(peek().line));
    }
}

void Parser::setTokens(vector<Token> t) {
    tokens = deque<Token>(t.begin(), t.end());
    lastLine = 1;
}

/* =======================
 * Assignment
 * ======================= */

unique_ptr<Assignment> Parser::parseAssignment(int line) {
    Token assigneeVariable = expectDynamic(DynamicTokenType::VARIABLE);
    expectStatic(StaticTokenType::EQUALS);
    Token assignVariable = expectDynamic(DynamicTokenType::VARIABLE);
    Token op = expectOneOfStatic(
        {StaticTokenType::PLUS, StaticTokenType::MINUS});
    Token constant = expectDynamic(DynamicTokenType::CONSTANT);

    return make_unique<Assignment>(
        assigneeVariable.getStringValue(),
        assignVariable.getStringValue(),
        getOperator(op),
        constant.getIntValue(),
        line
    );
}


vector<unique_ptr<Statement>>
LWParser::parse(vector<Token> tokensVec) {
    setTokens(move(tokensVec));
    return parseLW();
}

vector<unique_ptr<Statement>> LWParser::parseLW() {
    vector<unique_ptr<Statement>> statements;

    while (!isAtEnd()) {
        const Token& currentToken = peek();
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
                    return statements;

                case StaticTokenType::SEMICOLON:
                    consumeToken();
                    break;

                default:
                    throw runtime_error(
                        "Unexpected static token at line " +
                        to_string(currentToken.line));
            }
        } else if (currentToken.isDynamic()) {
            statements.push_back(
                parseAssignment(currentToken.line));
        } else {
            throw runtime_error(
                "Unexpected token at line " +
                to_string(currentToken.line));
        }
    }

    return statements;
}

unique_ptr<Loop> LWParser::parseLoop() {
    balancedIteration.push_back(StaticTokenType::LOOP);
    expectStatic(StaticTokenType::LOOP);

    Token conditionToken = expectOneOfDynamic(
        {DynamicTokenType::VARIABLE, DynamicTokenType::CONSTANT});
    expectStatic(StaticTokenType::DO);

    auto body = parseLW();
    expectStatic(StaticTokenType::END);
    validateSemicolon();

    return make_unique<Loop>(
        conditionToken.getDynamic().type ==
            DynamicTokenType::CONSTANT,
        conditionToken.getDynamic().value,
        move(body),
        conditionToken.line
    );
}

unique_ptr<While> LWParser::parseWhile() {
    balancedIteration.push_back(StaticTokenType::WHILE);
    expectStatic(StaticTokenType::WHILE);

    Token variableToken =
        expectDynamic(DynamicTokenType::VARIABLE);
    expectStatic(StaticTokenType::GREATER_THAN);
    Token constantToken =
        expectDynamic(DynamicTokenType::CONSTANT);
    expectStatic(StaticTokenType::DO);

    auto body = parseLW();
    expectStatic(StaticTokenType::END);
    validateSemicolon();

    return make_unique<While>(
        variableToken.getDynamic().value,
        stoi(constantToken.getDynamic().value),
        move(body),
        variableToken.line
    );
}

void LWParser::parseEnd() {
    if (!isBalancedStatementSequence(
            {StaticTokenType::LOOP,
             StaticTokenType::WHILE})) {
        throw runtime_error("Unmatched END statement");
    }
    expectStatic(StaticTokenType::END);
}

void LWParser::validateClosingSequence(int line) {
    if (!isBalancedStatementSequence(
            {StaticTokenType::LOOP,
             StaticTokenType::WHILE})) {
        throw runtime_error(
            "Unmatched END statement at line " +
            to_string(line));
    }
}

bool LWParser::isBalancedStatementSequence(
    initializer_list<StaticTokenType> expectedTypes) {
    for (const auto type : expectedTypes) {
        if (!balancedIteration.empty() &&
            balancedIteration.back() == type && encounteredEnd) {
            balancedIteration.pop_back();
            return true;
        }
    }
    return false;
}

vector<unique_ptr<Statement>>
GOTOParser::parse(vector<Token> tokensVec) {
    containsHalt_ = false;
    setTokens(move(tokensVec));
    return parseGoto();
}

vector<unique_ptr<Statement>> GOTOParser::parseGoto() {
    vector<unique_ptr<Statement>> statements;

    while (!isAtEnd()) {
        const Token& first = peek();
        int lineDifference = first.line - lastLine - 1;
        for (int i = 0; i < lineDifference; ++i) {
            statements.push_back(nullptr);
        }

        Token marker = expectDynamic(DynamicTokenType::MARKER);
        int markerLine = marker.line;
        markerLineMap_[marker.getStringValue()] = markerLine;

        expectStatic(StaticTokenType::COLON);

        if (peek().isStatic()) {
            switch (peek().getStatic().type) {
                case StaticTokenType::IF:
                    statements.push_back(
                        parseIf(markerLine,
                                marker.getIntValue()));
                    break;

                case StaticTokenType::GOTO:
                    statements.push_back(
                        parseGotoStatement(markerLine,
                                           marker.getIntValue()));
                    break;

                case StaticTokenType::HALT:
                    statements.push_back(
                        parseHalt(markerLine,
                                  marker.getIntValue()));
                    break;

                default:
                    throw runtime_error("Invalid GOTO statement");
            }
        } else {
            statements.push_back(
                parseAssignment(markerLine));
        }
        validateSemicolon();
    }

    checkGotoValues();
    return statements;
}

unique_ptr<Halt>
GOTOParser::parseHalt(int line, int markerLine) {
    expectStatic(StaticTokenType::HALT);
    containsHalt_ = true;
    return make_unique<Halt>(markerLine, line);
}

unique_ptr<If>
GOTOParser::parseIf(int line, int markerLine) {
    expectStatic(StaticTokenType::IF);
    Token variable =
        expectDynamic(DynamicTokenType::VARIABLE);
    expectStatic(StaticTokenType::EQUALS);
    Token constant =
        expectDynamic(DynamicTokenType::CONSTANT);
    expectStatic(StaticTokenType::THEN);
    expectStatic(StaticTokenType::GOTO);
    Token gotoMarker =
        expectDynamic(DynamicTokenType::MARKER);

    gotoValuesMap_[gotoMarker.getStringValue()] = line;

    return make_unique<If>(
        variable.getStringValue(),
        constant.getIntValue(),
        gotoMarker.getStringValue(),
        markerLine,
        line
    );
}

unique_ptr<Goto>
GOTOParser::parseGotoStatement(int line, int markerLine) {
    expectStatic(StaticTokenType::GOTO);
    Token gotoMarker =
        expectDynamic(DynamicTokenType::MARKER);
    gotoValuesMap_[gotoMarker.getStringValue()] = line;

    return make_unique<Goto>(
        gotoMarker.getStringValue(),
        markerLine,
        line
    );
}

void GOTOParser::checkGotoValues() {
    for (const auto& [markerName, gotoLine] : gotoValuesMap_) {
        if (markerLineMap_.find(markerName) == markerLineMap_.end()) {
            throw std::runtime_error(
                "No line with goto marker value " + markerName +
                " found (referenced at line " + std::to_string(gotoLine) + ")"
            );
        }
    }
}

std::map<std::string, int> GOTOParser::getMarkerLineMap() {
    return markerLineMap_;
}

