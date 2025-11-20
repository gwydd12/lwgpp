#ifndef LWGPP_LWSCANNER_H
#define LWGPP_LWSCANNER_H

#include "Scanner.h"
#include <regex>
#include <unordered_map>
#include <stdexcept>

/**
 * LWScanner – implementation for loop while language
 */
class LWScanner : public Scanner {
private:
    /**
    strong regex (const, static, RAII-valid)
    **/
    static const std::regex KEYWORDS;

    /**
     strongly typed enum + table (static, RAII-free)
    **/
    static const std::unordered_map<std::string, TokenType> KEYWORD_TABLE;

public:
    explicit LWScanner(const std::string &src)
        : Scanner(src) {
    }

protected:
    bool isKeyword(const std::string &word) override {
        return std::regex_match(word, KEYWORDS);
    }

    void addKeywordToken(const std::string &word) override {
        // modern C++: if mit initializer + auto
        if (auto it = KEYWORD_TABLE.find(word); it != KEYWORD_TABLE.end()) {
            tokens.emplace_back(it->second, currentLine);
        } else {
            throw std::invalid_argument("Invalid token: " + word);
        }
    }
};

#endif
