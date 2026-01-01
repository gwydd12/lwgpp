#include "GotoParser.h"

#include <functional>
#include <unordered_map>

using namespace goto_parser;

std::vector<std::unique_ptr<Statement>> GotoParser::parse(std::vector<Token> tokens) {
    containsHalt_ = false;
    setTokens(std::move(tokens)); // the expression std::move(tokens) is an xvalue of type std::vector<Token> (no identity, can be moved from)
    return parseGoto();
}

std::vector<std::unique_ptr<Statement>> GotoParser::parseGoto() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (!isAtEnd()) {
        const Token& first = peek();
        fillStatementsWithNops(statements, first.line, lastLine);

        const Token& marker = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::MARKER>();
        const int markerLine = marker.line;
        markerLineMap_[marker.getStringValue()] = markerLine;

        expectAndConsumeToken<StaticTokenType, StaticTokenType::COLON>();

        if (peek().is<StaticToken>()) {
            // Router pattern to dispatch parsing based on the next token type
            // Uses std::function to define a handler type for parsing different statements
            // The Handler type takes the marker line and marker value as parameters and returns a unique_ptr to a Statement
            // This could also be solved via switch statements.
            using Handler = std::function<std::unique_ptr<Statement>(int markerLine, int markerValue)>;


            std::unordered_map<StaticTokenType, Handler> router {
                    { StaticTokenType::IF,   [this](const int ml, const int mv) { return parseIf(ml, mv); } },
                    { StaticTokenType::GOTO, [this](const int ml, const int mv) { return parseGotoStatement(ml, mv); } },
                    { StaticTokenType::HALT, [this](const int ml, const int mv) { return parseHalt(ml, mv); } },
                };

            // Dispatch to the appropriate parsing function based on the next token type
            // It searches for the token type of the current token in the router map and
            // calls the corresponding handler function if found.
            const auto type = peek().getType<StaticTokenType>();
            if (auto it = router.find(type); it != router.end()) {
                statements.push_back(it->second(markerLine, marker.getIntValue()));
            } else {
                throw std::runtime_error("Invalid GOTO statement");
            }
        } else {
            statements.push_back(
                parseAssignment(markerLine));
        }
        validateSemicolon();
    }

    validateGotoMarkerValues();
    return statements;
}

std::unique_ptr<Halt> GotoParser::parseHalt(int line, int markerLine) {
    expectAndConsumeToken<StaticTokenType, StaticTokenType::HALT>();
    containsHalt_ = true;
    return std::make_unique<Halt>(markerLine, line);
}

std::unique_ptr<If> GotoParser::parseIf(int line, int markerLine) {
    expectAndConsumeToken<StaticTokenType, StaticTokenType::IF>();
    const Token& variable = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::VARIABLE>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::EQUALS>();
    const Token& constant = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::CONSTANT>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::THEN>();
    expectAndConsumeToken<StaticTokenType, StaticTokenType::GOTO>();
    const Token& gotoMarker = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::MARKER>();

    gotoValuesMap_[gotoMarker.getStringValue()] = line;

    return std::make_unique<If>(
        variable.getStringValue(),
        constant.getIntValue(),
        gotoMarker.getStringValue(),
        markerLine,
        line
    );
}

std::unique_ptr<Goto> GotoParser::parseGotoStatement(int line, int markerLine) {
    expectAndConsumeToken<StaticTokenType, StaticTokenType::GOTO>();
    const Token& gotoMarker = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::MARKER>();
    gotoValuesMap_[gotoMarker.getStringValue()] = line;

    return std::make_unique<Goto>(
        gotoMarker.getStringValue(),
        markerLine,
        line
    );
}

/**
 * Validates that all Goto statements reference existing markers.
 *
 */
void GotoParser::validateGotoMarkerValues() {
    for (const auto& [markerName, gotoLine] : gotoValuesMap_) {
        if (!markerLineMap_.contains(markerName)) {
            throw std::runtime_error(
                "No line with goto marker value " + markerName +
                " found (referenced at line " + std::to_string(gotoLine) + ")"
            );
        }
    }
}

/**
 * Allows for retrieval of the marker to line number mapping.
 *
 * @return Map of marker and their corresponding line numbers
 */
std::map<std::string, int> GotoParser::getMarkerLineMap() {
    return markerLineMap_;
}

void GotoParser::fillStatementsWithNops(std::vector<std::unique_ptr<Statement>>& statements, const int firstLine, const int lastLine) {
    const int lineDifference = firstLine - lastLine - 1;
    for (int i = 0; i < lineDifference; ++i) {
        statements.push_back(nullptr);
    }
}