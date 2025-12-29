#ifndef LWGPP_LEXER_H
#define LWGPP_LEXER_H

#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include "../error/ErrorHandler.h"
#include "../token/Token.h"

/**
 * Abstract base Scanner class
 *
 * Responsibilities:
 * - Holds source code string
 * - Splits source code into lines and words
 * - Recognizes variables, constants, and delegates keywords to derived scanners
 * - Collects tokens
 *
 * Concepts used:
 * - virtual functions and pure virtual functions for polymorphism
 * - RAII: std::vector and std::string manage memory automatically
 * - std::regex for pattern matching
 * - move semantics in constructor
 */
class Scanner {
protected:
    std::string source;
    std::vector<Token> tokens;
    int currentLine = 1;

    // Patterns for variable names and constants
    const std::regex IDENTIFIER{R"(x\d+)"}; // matches x1, x2, ...
    const std::regex CONSTANT{R"(\d+)"};    // matches integers

public:
    /**
     * Move Constructor
     * @param src input source code
     *
     * Concepts:
     * - explicit to prevent implicit conversions
     * - std::move to efficiently move source string into member
     *      - Avoids unnecessary copies
     *      - Move constructor of std::string is used
     * - RAII: std::string manages its own memory
     */
    explicit Scanner(std::string src)
        : source(std::move(src)) {
    }

    /**
     * Virtual destructor for polymorphic cleanup
     */
    virtual ~Scanner() = default;

    /**
     * Scan the entire source program
     * @return vector of tokens
     *
     * Concepts:
     * - std::getline to iterate lines
     * - Early return if errors occurred
     */
    std::vector<Token> scanProgram() {
        std::istringstream stream(source);
        std::string line;

        while (std::getline(stream, line)) {
            scanLine(line);
            currentLine++;
        }

        if (ErrorHandler::hadError)
            return {};
        return tokens;   // NRVO (named return value optimization)
    }

protected:
    /**
     * Scan a single line
     * @param line the input line
     *
     * Concepts:
     * - std::istringstream for tokenizing by whitespace
     * - loop through words, handle tokens
     */
    void scanLine(std::string line) {
        line = stripComments(line);

        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            if (matchToken(word)) continue;
            if (checkSemicolon(word)) continue;

            // Report unexpected token
            ErrorHandler::report(currentLine, "Unexpected token '" + word + "'");
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
    static std::string stripComments(const std::string &line) {
        size_t pos = line.find("//");
        return (pos != std::string::npos) ? line.substr(0, pos) : line;
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
    bool checkSemicolon(const std::string &word) {
        if (!word.empty() && word.back() == ';') {
            std::string tokenPart = word.substr(0, word.size() - 1);
            if (!tokenPart.empty() && matchToken(tokenPart)) {
                tokens.emplace_back(StaticTokenType::SEMICOLON, currentLine);
                return true;
            } else {
                ErrorHandler::report(currentLine, "Unexpected token '" + word + "'");
                return true;
            }
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
    virtual bool matchToken(const std::string &word) {
        if (isKeyword(word)) {
            addKeywordToken(word);
            return true;
        }

        if (std::regex_match(word, IDENTIFIER)) {
            std::string value = word;
            tokens.emplace_back(DynamicTokenType::VARIABLE, value, currentLine);
            return true;
        }

        if (std::regex_match(word, CONSTANT)) {
            std::string value = word;
            tokens.emplace_back(DynamicTokenType::CONSTANT, value, currentLine);
            return true;
        }

        return false;
    }

    /**
     * Pure virtual function to check for keywords
     * Pure virtual function to be implemented by derived classes
     */
    virtual bool isKeyword(const std::string &word) = 0;

    /**
     * Pure virtual function to add a keyword token
     * Pure virtual function to be implemented by derived classes
     */
    virtual void addKeywordToken(const std::string &word) = 0;
};

#endif
