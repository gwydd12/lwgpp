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
public:
    /**
     * Constructor
     * @param src input source code
     *
     * Concepts:
     * - explicit: prevents implicit conversion from string to LWScanner
     * - delegates to base class constructor (move constructor in base class)
     */
    explicit LWScanner(const std::string &src)
        : Scanner(src) {
    }

protected:
    bool isKeyword(const std::string &word) override;
    void addKeywordToken(const std::string &word) override;

private:
    /**
     * Regex for all language keywords
     * - static: single instance for all objects
     * - const: immutable pattern
     * - RAII valid: automatically managed by compiler
     */
    static const std::regex keywords_;

    /**
     * Static lookup table for keywords
     * - unordered_map: fast O(1) lookup
     * - uses strongly typed enum StaticTokenType
     */
    static const std::unordered_map<std::string, StaticTokenType> keyword_table_;
};

#endif
