#ifndef LWGPP_LWSCANNER_H
#define LWGPP_LWSCANNER_H

#include "Scanner.h"
#include <regex>
#include <unordered_map>
#include <stdexcept>

/**
 * LWScanner – Scanner for Loop/While language
 *
 * Responsibilities:
 * - Recognizes keywords: Loop, While, Do, End
 * - Recognizes operators: =, +, -, >, ;
 * - Produces tokens for the parser
 *
 * Concepts used:
 * - static const members (RAII-valid initialization)
 * - unordered_map for fast lookup
 * - override for polymorphism
 * - auto in if-initializer
 */
class LWScanner : public Scanner {
private:
    /**
     * Regex for all language keywords
     * - static: single instance for all objects
     * - const: immutable pattern
     * - RAII valid: automatically managed by compiler
     */
    static const std::regex KEYWORDS;

    /**
     * Static lookup table for keywords
     * - unordered_map: fast O(1) lookup
     * - uses strongly typed enum TokenType
     */
    static const std::unordered_map<std::string, TokenType> KEYWORD_TABLE;

public:
    /**
     * Constructor
     * @param src input source code
     *
     * Concepts:
     * - explicit: prevents implicit conversion from string to LWScanner
     * - delegates to base class constructor
     */
    explicit LWScanner(const std::string &src)
        : Scanner(src) {
    }

protected:
    /**
     * Checks if a word is a keyword.
     *
     * @param word input string
     * @return true if word is a keyword
     *
     * Concepts:
     * - override: overrides virtual Scanner::isKeyword
     * - std::regex_match: matches string against regex
     */
    bool isKeyword(const std::string &word) override {
        return std::regex_match(word, KEYWORDS);
    }

    /**
     * Adds a token for a recognized keyword.
     * Throws exception if the keyword is invalid.
     *
     * @param word the keyword string
     *
     * Concepts:
     * - auto in if-initializer (C++17)
     * - unordered_map lookup
     * - emplace_back: efficient in-place construction of tokens
     * - exception handling: throws std::invalid_argument
     */
    void addKeywordToken(const std::string &word) override {
        if (auto it = KEYWORD_TABLE.find(word); it != KEYWORD_TABLE.end()) {
            tokens.emplace_back(it->second, currentLine);
        } else {
            throw std::invalid_argument("Invalid token: " + word);
        }
    }
};

#endif
