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
const std::unordered_map<std::string, TokenType> LWScanner::KEYWORD_TABLE = {
    {"Loop", TokenType::LOOP},
    {"While", TokenType::WHILE},
    {"Do", TokenType::DO},
    {"End", TokenType::END},
    {"=", TokenType::EQUALS},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {">", TokenType::GREATER_THAN},
    {";", TokenType::SEMICOLON}
};