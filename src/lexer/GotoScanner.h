#ifndef LWGPP_GOTOSCANNER_H
#define LWGPP_GOTOSCANNER_H

#include "Scanner.h"
#include <regex>
#include <unordered_map>

namespace lexer::goto_lang {
    /**
     * GotoScanner – C++ Scanner for GOTO-style language
     *
     * Responsibilities:
     * - Recognizes keywords: Goto, If, Then, Halt, operators (=, +, -, ;)
     * - Recognizes markers: M1, M2, … optionally followed by ':'
     * - Produces Tokens for the parser
     *
     * Concepts used:
     * - static const members for single initialization (RAII-friendly)
     * - unordered_map as a lookup table for fast keyword matching
     * - override for polymorphism
     * - exception handling for invalid markers
     */
    class GotoScanner : public Scanner {
    public:
        /**
         * Constructor
         *
         * @param src the source code string to scan
         *
         * Concepts used:
         * - explicit to prevent implicit conversions
         * - move semantics from Scanner constructor
         */
        explicit GotoScanner(const std::string &src)
            : Scanner(src) {
        }

    protected:
        bool isKeyword(const std::string &word) override;
        void addKeywordToken(const std::string &word) override;

        static bool isMarker(const std::string &word);
        void addMarkerToken(const std::string &word);
        bool matchToken(const std::string &word) override;

    private:
        /**
         * Regex for all language keywords (static, const, RAII valid)
         */
        static const std::regex keywords_;

        /**
         * Static lookup table mapping keyword strings to TokenType
         * (unordered_map, strongly typed enum usage)
         */
        static const std::unordered_map<std::string, StaticTokenType> keyword_table_;

        /**
         * Regex for markers (M1, M2, ... optionally ending with ':')
         */
        static const std::regex marker_;
    };
}


#endif
