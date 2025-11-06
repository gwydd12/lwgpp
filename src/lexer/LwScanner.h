#ifndef LWGPP_LWSCANNER_H
#define LWGPP_LWSCANNER_H

#include "Scanner.h"
#include <regex>
#include <stdexcept>

/**
 * LWScanner – implementation for loop while language
 */
class LWScanner : public Scanner {
private:
    const std::regex KEYWORDS{R"(Loop|While|Do|End|=|\+|\-|>|;)"};

public:
    explicit LWScanner(const std::string& src)
        : Scanner(src) {}

protected:
    bool isKeyword(const std::string& word) override {
        return std::regex_match(word, KEYWORDS);
    }

    void addKeywordToken(const std::string& word) override {
        if (word == "Loop") {
            tokens.emplace_back(TokenType::LOOP, currentLine);
        } else if (word == "While") {
            tokens.emplace_back(TokenType::WHILE, currentLine);
        } else if (word == "Do") {
            tokens.emplace_back(TokenType::DO, currentLine);
        } else if (word == "End") {
            tokens.emplace_back(TokenType::END, currentLine);
        } else if (word == "=") {
            tokens.emplace_back(TokenType::EQUALS, currentLine);
        } else if (word == "+") {
            tokens.emplace_back(TokenType::PLUS, currentLine);
        } else if (word == "-") {
            tokens.emplace_back(TokenType::MINUS, currentLine);
        } else if (word == ">") {
            tokens.emplace_back(TokenType::GREATER_THAN, currentLine);
        } else if (word == ";") {
            tokens.emplace_back(TokenType::SEMICOLON, currentLine);
        } else {
            throw std::invalid_argument("Invalid token: " + word);
        }
    }
};

#endif // LWGPP_LWSCANNER_H
