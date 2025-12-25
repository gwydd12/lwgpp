//
// Created by philipp.hennken on 20.11.2025.
//

#include "GotoScanner.h"

const std::regex GotoScanner::KEYWORDS{
    R"(Goto|If|Then|Halt|=|\+|-|;)"
};

/**
 * STL Containers -> Unordered Map
 */
const std::unordered_map<std::string, TokenType> GotoScanner::KEYWORD_TABLE = {
    {"Goto", TokenType::GOTO},
    {"If", TokenType::IF},
    {"Then", TokenType::THEN},
    {"Halt", TokenType::HALT},
    {"=", TokenType::EQUALS},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {";", TokenType::SEMICOLON}
};

const std::regex GotoScanner::MARKER{
    R"(M\d+:?)"
};