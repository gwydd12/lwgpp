//
// Created by philipp.hennken on 04.11.2025.
//

#include "LwScanner.h"

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
bool LWScanner::isKeyword(const std::string &word) {
    return std::regex_match(word, keywords_);
}

/**
 * Adds a token for a recognized keyword.
 * Throws exception if the keyword is invalid.
 *
 * @param word the keyword string
 *
 * Concepts:
 * - auto in if-initializer (C++17)
 *     - auto deduces at compile-time the iterator type from unordered_map
 * - unordered_map lookup
 * - emplace_back: efficient in-place construction of tokens
 * - exception handling: throws std::invalid_argument
 */
void LWScanner::addKeywordToken(const std::string &word){
    if (const auto it = keyword_table_.find(word); it != keyword_table_.end()) {
        tokens_.emplace_back(it->second, currentLine_);
    } else {
        throw std::invalid_argument("Invalid token: " + word);
    }
}

const std::regex LWScanner::keywords_{
    R"(Loop|While|Do|End|=|\+|\-|>|;)"
};

/**
 * STL Containers -> Unordered Map
 * - Unordered associative container
 */
const std::unordered_map<std::string, StaticTokenType> LWScanner::keyword_table_ = {
    {"Loop", StaticTokenType::LOOP},
    {"While", StaticTokenType::WHILE},
    {"Do", StaticTokenType::DO},
    {"End", StaticTokenType::END},
    {"=", StaticTokenType::EQUALS},
    {"+", StaticTokenType::PLUS},
    {"-", StaticTokenType::MINUS},
    {">", StaticTokenType::GREATER_THAN},
    {";", StaticTokenType::SEMICOLON}
};