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
const std::unordered_map<std::string, StaticTokenType> GotoScanner::KEYWORD_TABLE = {
    {"Goto", StaticTokenType::GOTO},
    {"If", StaticTokenType::IF},
    {"Then", StaticTokenType::THEN},
    {"Halt", StaticTokenType::HALT},
    {"=", StaticTokenType::EQUALS},
    {"+", StaticTokenType::PLUS},
    {"-", StaticTokenType::MINUS},
    {";", StaticTokenType::SEMICOLON}
};

const std::regex GotoScanner::MARKER{
    R"(M\d+:?)"
};