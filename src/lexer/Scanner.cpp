//
// Created by philipp.hennken on 30.10.2025.
//

#include "Scanner.h"

#include "../error/ErrorHandler.h"

/**
 * Scan the entire source program
 * @return vector of tokens
 *
 * Concepts:
 * - std::getline to iterate lines
 * - Early return if errors occurred
 */
std::vector<Token> Scanner::scanProgram() {
    std::istringstream stream(source_);
    std::string line;

    while (std::getline(stream, line)) {
        scanLine(line);
        currentLine_++;
    }

    if (ErrorHandler::hadError)
        return {};
    return tokens_;   // NRVO (named return value optimization)
}

/**
 * Scan a single line
 * @param line the input line
 *
 * Concepts:
 * - std::istringstream for tokenizing by whitespace
 * - loop through words, handle tokens
 */
void Scanner::scanLine(std::string line) {
    line = stripComments(line);

    std::istringstream iss(line);
    std::string word;
    while (iss >> word) {
        if (matchToken(word)) continue;
        if (checkSemicolon(word)) continue;

        // Report unexpected token
        ErrorHandler::report(currentLine_, "Unexpected token '" + word + "'");
    }
}

/**
 * Remove comments from a line
 * @param line the input line
 * @return line without comments
 *
 * Concepts:
 * - static member function
 *      - Does not depend on instance state
 *      - Can be called without an object
 * - std::string::find and substr
 */
std::string Scanner::stripComments(const std::string &line) {
    const size_t pos = line.find("//");
    return pos != std::string::npos ? line.substr(0, pos) : line;
}

/**
 * Check if a word ends with a semicolon
 * @param word the input string
 * @return true if semicolon was processed
 *
 * Concepts:
 * - string manipulation (back, substr)
 * - calls matchToken recursively for the rest
 * - adds SEMICOLON token
 */
bool Scanner::checkSemicolon(const std::string &word) {
    if (!word.empty() && word.back() == ';') {
        const std::string tokenPart = word.substr(0, word.size() - 1);
        if (!tokenPart.empty() && matchToken(tokenPart)) {
            tokens_.emplace_back(StaticTokenType::SEMICOLON, currentLine_);
            return true;
        }
        ErrorHandler::report(currentLine_, "Unexpected token '" + word + "'");
        return true;
    }
    return false;
}

/**
 * Try to match a word to a token
 * @param word the input string
 * @return true if matched
 *
 * Concepts:
 * - virtual to allow derived classes to override
 * - std::regex_match for IDENTIFIER and CONSTANT
 * - std::stoi to convert string to integer
 * - emplace_back for efficient Token construction
 *      - Appends new tokens to the end of the tokens vector
 *      - Usually uses placement new to construct Token in place (std::allocator_traits::construct)
 */
bool Scanner::matchToken(const std::string &word) {
    if (isKeyword(word)) {
        addKeywordToken(word);
        return true;
    }

    if (std::regex_match(word, IDENTIFIER)) {
        tokens_.emplace_back(DynamicTokenType::VARIABLE, word, currentLine_);
        return true;
    }

    if (std::regex_match(word, CONSTANT)) {
        tokens_.emplace_back(DynamicTokenType::CONSTANT, word, currentLine_);
        return true;
    }

    return false;
}