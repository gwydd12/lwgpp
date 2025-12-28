//
// Created by philipp.hennken on 04.11.2025.
//

#include "LwScanner.h"

const std::regex LWScanner::KEYWORDS{
    R"(Loop|While|Do|End|=|\+|\-|>|;)"
};

/**
 * STL Containers -> Unordered Map
 */
const std::unordered_map<std::string, StaticTokenType> LWScanner::KEYWORD_TABLE = {
    {"Loop", StaticTokenType::LOOP},
    {"While", StaticTokenType::WHILE},
    {"Do", StaticTokenType::DO},
    {"End", StaticTokenType::END},
    {"=", StaticTokenType::EQUALS},
    {"+", StaticTokenType::PLUS},
    {"-", StaticTokenType::MINUS},
    {">", StaticTokenType::GREATER_THAN},
    {";", StaticTokenType::SEMICOLON}
};