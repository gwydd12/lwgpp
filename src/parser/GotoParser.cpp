#include "GotoParser.h"

std::vector<std::unique_ptr<Statement>> GOTOParser::parse(std::vector<Token> toks) {
    containsHalt_ = false;
    setTokens(std::move(toks));
    return parseGoto();
}

std::vector<std::unique_ptr<Statement>> GOTOParser::parseGoto() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (!isAtEnd()) {
        const Token& first = peek();
        const int lineDifference = first.line - lastLine - 1;
        for (int i = 0; i < lineDifference; ++i) {
            statements.push_back(nullptr);
        }

        const Token& marker = expectAndConsumeToken<DynamicTokenType, DynamicTokenType::MARKER>();
        const int markerLine = marker.line;
        markerLineMap_[marker.getStringValue()] = markerLine;

        expectAndConsumeToken<StaticTokenType, StaticTokenType::COLON>();

        if (peek().is<StaticToken>()) {
            switch (peek().getType<StaticTokenType>()) {
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
                    throw std::runtime_error("Invalid GOTO statement");
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

std::unique_ptr<Halt>
GOTOParser::parseHalt(int line, int markerLine) {
    expectAndConsumeToken<StaticTokenType, StaticTokenType::HALT>();
    containsHalt_ = true;
    return std::make_unique<Halt>(markerLine, line);
}

std::unique_ptr<If>
GOTOParser::parseIf(int line, int markerLine) {
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

std::unique_ptr<Goto>
GOTOParser::parseGotoStatement(int line, int markerLine) {
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
 *
 */
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

/**
 * Allows for retrieval of the marker to line number mapping.
 *
 * @return Map of marker and their corresponding line numbers
 */
std::map<std::string, int> GOTOParser::getMarkerLineMap() {
    return markerLineMap_;
}