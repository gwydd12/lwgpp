//
// Created by philipp.hennken on 30.10.2025.
//

#ifndef LWGPP_LEXER_H
#define LWGPP_LEXER_H
#include <regex>
#include <string>
#include <vector>
#include "../error/ErrorHandler.h"
#include "../token/Token.h"



class Scanner {
protected:
    std::string source;
    std::vector<Token> tokens;
    int currentLine = 1;

    // Regex patterns
    const std::regex IDENTIFIER{R"(x\d+)"};
    const std::regex CONSTANT{R"(\d+)"};

public:
    explicit Scanner(std::string src)
        : source(std::move(src)) {}

    virtual ~Scanner() = default;

    // Main entry point
    std::vector<Token> scanProgram() {
        std::istringstream stream(source);
        std::string line;

        while (std::getline(stream, line)) {
            scanLine(line);
            currentLine++;
        }

        if (ErrorHandler::hadError)
            return {};
        return tokens;
    }

protected:
    void scanLine(std::string line) {
        line = stripComments(line);

        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            if (matchToken(word)) continue;
            if (checkSemicolon(word)) continue;
            ErrorHandler::report(currentLine, "Unexpected token '" + word + "'");
        }
    }

    static std::string stripComments(const std::string& line) {
        size_t pos = line.find("//");
        return (pos != std::string::npos) ? line.substr(0, pos) : line;
    }

    bool checkSemicolon(const std::string& word) {
        if (!word.empty() && word.back() == ';') {
            std::string tokenPart = word.substr(0, word.size() - 1);
            if (!tokenPart.empty() && matchToken(tokenPart)) {
                tokens.emplace_back(TokenType::SEMICOLON, currentLine);
                return true;
            } else {
                ErrorHandler::report(currentLine, "Unexpected token '" + word + "'");
                return true;
            }
        }
        return false;
    }

    bool matchToken(const std::string& word) {
        if (isKeyword(word)) {
            addKeywordToken(word);
            return true;
        }

        if (std::regex_match(word, IDENTIFIER)) {
            int value = std::stoi(word.substr(1));
            tokens.emplace_back(TokenType::VARIABLE, currentLine, value);
            return true;
        }

        if (std::regex_match(word, CONSTANT)) {
            int value = std::stoi(word);
            tokens.emplace_back(TokenType::CONSTANT, currentLine, value);
            return true;
        }

        return false;
    }

    // Abstract methods
    virtual bool isKeyword(const std::string& word) = 0;
    virtual void addKeywordToken(const std::string& word) = 0;
};


#endif //LWGPP_LEXER_H