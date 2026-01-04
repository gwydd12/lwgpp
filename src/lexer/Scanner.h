#ifndef LWGPP_LEXER_H
#define LWGPP_LEXER_H

#include <regex>
#include <string>
#include <vector>
#include "../token/Token.h"

namespace lexer {
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
            : source_(std::move(src)) {
        }
        virtual ~Scanner() = default;
        std::vector<Token> scanProgram();

    protected:
        std::string source_;
        std::vector<Token> tokens_;
        int currentLine_ = 1;

        const std::regex IDENTIFIER{R"(x\d+)"}; // matches x1, x2, ...
        const std::regex CONSTANT{R"(\d+)"};    // matches integers

        void scanLine(std::string line);
        static std::string stripComments(const std::string &line);
        bool checkSemicolon(const std::string &word);
        virtual bool matchToken(const std::string &word);
        virtual bool isKeyword(const std::string &word) = 0;
        virtual void addKeywordToken(const std::string &word) = 0;
    };
}

#endif
