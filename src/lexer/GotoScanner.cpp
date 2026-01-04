#include "GotoScanner.h"

using namespace lexer::goto_lang;
/**
 * Check if a word matches any of the language keywords.
 *
 * @param word the input string
 * @return true if it is a keyword, false otherwise
 *
 * Concepts used:
 * - override for polymorphic behavior
 * - std::regex_match for pattern matching
 */
bool GotoScanner::isKeyword(const std::string &word) {
    return std::regex_match(word, keywords_);
}

/**
 * Add a token for a recognized keyword.
 * Throws exception if the keyword is invalid.
 *
 * @param word the keyword string
 *
 * Concepts used:
 * - auto with if initializer (C++17)
 * - unordered_map lookup
 * - exception handling
 */
void GotoScanner::addKeywordToken(const std::string &word) {
    if (const auto it = keyword_table_.find(word); it != keyword_table_.end()) {
        tokens_.emplace_back(it->second, currentLine_);
    } else {
        throw std::invalid_argument("Invalid token: " + word);
    }
}

/**
 * Check if a word matches the marker pattern (e.g., M1, M2:).
 *
 * @param word the input string
 * @return true if it is a marker
 *
 * Concepts used:
 * - const member function (does not modify object)
 * - std::regex_match
 */
bool GotoScanner::isMarker(const std::string &word) {
    return std::regex_match(word, marker_);
}

/**
 * Add a token for a recognized marker.
 * If the marker ends with ':', adds a COLON token as well.
 * Throws exception if marker value is invalid.
 *
 * @param word the marker string
 *
 * Concepts used:
 * - exception handling (try/catch)
 * - string manipulation (substr, size)
 * - emplace_back with Token construction
 */
void GotoScanner::addMarkerToken(const std::string &word) {
    try {
        constexpr size_t start = 1;
        const size_t end = word.back() == ':' ? word.size() - 1 : word.size();
        std::string value = word.substr(start, end - start);

        tokens_.emplace_back(DynamicTokenType::MARKER, value, currentLine_);

        if (word.back() == ':') {
            tokens_.emplace_back(StaticTokenType::COLON, currentLine_);
        }
    } catch (const std::exception &) {
        throw std::invalid_argument("Marker value too big or invalid: " + word);
    }
}

/**
 * Match a word to a token.
 * Checks keywords, then markers, then falls back to Scanner defaults (variables/constants).
 *
 * @param word the input string
 * @return true if a token was recognized and added
 *
 * Concepts used:
 * - override for polymorphism
 * - calling base class method with Scanner::matchToken
 */
bool GotoScanner::matchToken(const std::string &word) {
    if (isKeyword(word)) {
        addKeywordToken(word);
        return true;
    }

    if (isMarker(word)) {
        addMarkerToken(word);
        return true;
    }

    // Fallback to base Scanner token matching (variables, constants)
    return Scanner::matchToken(word);
}

const std::regex GotoScanner::keywords_{
    R"(Goto|If|Then|Halt|=|\+|-|;)"
};

/**
 * STL Containers -> Unordered Map
 */
const std::unordered_map<std::string, StaticTokenType> GotoScanner::keyword_table_ = {
    {"Goto", StaticTokenType::GOTO},
    {"If", StaticTokenType::IF},
    {"Then", StaticTokenType::THEN},
    {"Halt", StaticTokenType::HALT},
    {"=", StaticTokenType::EQUALS},
    {"+", StaticTokenType::PLUS},
    {"-", StaticTokenType::MINUS},
    {";", StaticTokenType::SEMICOLON}
};

const std::regex GotoScanner::marker_{
    R"(M\d+:?)"
};