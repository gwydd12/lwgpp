#ifndef LWGPP_GOTOSCANNER_H
#define LWGPP_GOTOSCANNER_H

#include "Scanner.h"
#include <regex>
#include <unordered_map>
#include <stdexcept>

/**
 * GotoScanner – C++ Scanner for GOTO-style language
 *
 * Responsibilities:
 * - Recognizes keywords: Goto, If, Then, Halt, operators (=, +, -, ;)
 * - Recognizes markers: M1, M2, … optionally followed by ':'
 * - Produces Tokens for the parser
 *
 * Concepts used:
 * - static const members for single initialization (RAII-friendly)
 * - unordered_map as a lookup table for fast keyword matching
 * - override for polymorphism
 * - exception handling for invalid markers
 */
class GotoScanner : public Scanner {
private:
    /**
     * Regex for all language keywords (static, const, RAII valid)
     */
    static const std::regex KEYWORDS;

    /**
     * Static lookup table mapping keyword strings to TokenType
     * (unordered_map, strongly typed enum usage)
     */
    static const std::unordered_map<std::string, StaticTokenType> KEYWORD_TABLE;

    /**
     * Regex for markers (M1, M2, ... optionally ending with ':')
     */
    static const std::regex MARKER;

public:
    /**
     * Constructor
     *
     * @param src the source code string to scan
     *
     * Concepts used:
     * - explicit to prevent implicit conversions
     * - move semantics from Scanner constructor
     */
    explicit GotoScanner(const std::string &src)
        : Scanner(src) {
    }

protected:
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
    bool isKeyword(const std::string &word) override {
        return std::regex_match(word, KEYWORDS);
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
    void addKeywordToken(const std::string &word) override {
        if (const auto it = KEYWORD_TABLE.find(word); it != KEYWORD_TABLE.end()) {
            tokens.emplace_back(it->second, currentLine);
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
    bool isMarker(const std::string &word) const {
        return std::regex_match(word, MARKER);
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
     * - std::stoi conversion
     * - emplace_back with Token construction
     */
    void addMarkerToken(const std::string &word) {
        try {
            // Extract number from marker string
            size_t start = 1;
            size_t end = word.back() == ':' ? word.size() - 1 : word.size();
            std::string value = word.substr(start, end - start);

            // Add marker token
            tokens.emplace_back(DynamicTokenType::MARKER, value, currentLine);

            // Add colon token if present
            if (word.back() == ':') {
                tokens.emplace_back(StaticTokenType::COLON, currentLine);
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
    bool matchToken(const std::string &word) override {
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

};

#endif
