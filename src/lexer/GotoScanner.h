#ifndef LWGPP_GOTOSCANNER_H
#define LWGPP_GOTOSCANNER_H

#include "Scanner.h"
#include <regex>
#include <unordered_map>
#include <stdexcept>

class GotoScanner : public Scanner {
private:
    /**
    strong regex (const, static, RAII-valid)
    **/
    static const std::regex KEYWORDS;

    /**
     strongly typed enum + table (static, RAII-free)
    **/
    static const std::unordered_map<std::string, TokenType> KEYWORD_TABLE;

    static const std::regex MARKER;

public:
    explicit GotoScanner(const std::string &src)
        : Scanner(src) {
    }

protected:
    bool isKeyword(const std::string &word) override {
        return std::regex_match(word, KEYWORDS);
    }

    void addKeywordToken(const std::string &word) override {
        if (auto it = KEYWORD_TABLE.find(word); it != KEYWORD_TABLE.end()) {
            tokens.emplace_back(it->second, currentLine);
        } else {
            throw std::invalid_argument("Invalid token: " + word);
        }
    }

    bool isMarker(const std::string &word) const {
        return std::regex_match(word, MARKER);
    }

    void addMarkerToken(const std::string &word) {
        try {
            size_t start = 1;
            size_t end = word.back() == ':' ? word.size() - 1 : word.size();
            int value = std::stoi(word.substr(start, end - start));

            tokens.emplace_back(TokenType::MARKER, currentLine, value);

            if (word.back() == ':') {
                tokens.emplace_back(TokenType::COLON, currentLine);
            }
        } catch (const std::exception &) {
            throw std::invalid_argument("Marker value too big or invalid: " + word);
        }
    }

    bool matchToken(const std::string &word) override {
        if (isKeyword(word)) {
            addKeywordToken(word);
            return true;
        }

        if (isMarker(word)) {
            addMarkerToken(word);
            return true;
        }

        return Scanner::matchToken(word);
    }

};

#endif
