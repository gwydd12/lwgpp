#ifndef LWGPP_TOKEN_H
#define LWGPP_TOKEN_H
#include <stdexcept>
#include <string>
#include <variant>

/**
 * Token definitions for the lexer and parser
 *
 * Responsibilities:
 * - Define static and dynamic token types
 * - Provide a Token struct that can hold either type
 * - Offer utility methods for token type checking and value retrieval
 *
 * Concepts used:
 * - auto keyword for type inference
 * - enum class for strong type safety
 * - std::variant for type-safe union of static and dynamic tokens (2 different types)
 * - std::string for dynamic token values 
 * - std::move to cast arguments to rvalue references and use move constructor
 * - std::runtime_error for exception handling
 * - std::holds_alternative to check the type held by std::variant
 * - std::get_if to safely retrieve the value from std::variant
 * - Exception handling for invalid token access
 */

enum class TokenCategory {STATIC, DYNAMIC};
enum class StaticTokenType {
    LOOP,
    WHILE,
    DO,
    END,
    EQUALS,
    PLUS,
    MINUS,
    GREATER_THAN,
    IF,
    THEN,
    GOTO,
    HALT,
    SEMICOLON,
    COLON,
};

enum class DynamicTokenType {
    VARIABLE,
    CONSTANT,
    MARKER
};

struct DynamicToken {
    DynamicTokenType type;
    std::string value; 
    DynamicToken(const DynamicTokenType t, std::string v): type(t), value(std::move(v)) {}
};

struct StaticToken {
    StaticTokenType type;
    explicit StaticToken(const StaticTokenType t): type(t) {}
};

/**
 * A Token can be either a static type or a dynamic type.
 * The static type does not carry a value, while the dynamic type does store a string value.
 * We use std::variant to represent a type-safe union of the two categories. (https://en.cppreference.com/w/cpp/utility/variant.html
 */
struct Token {
    std::variant<StaticToken, DynamicToken> value; // std::variant applied here
    int line;

    Token(StaticToken staticType, const int l): value(staticType), line(l) {}
    // Using std::move to avoid unnecessary copies of the string value.
    Token(DynamicToken dynamicType, const int l): value(std::move(dynamicType)), line(l) {}
    Token(StaticTokenType staticType, const int l): value(StaticToken(staticType)), line(l) {}
    Token(DynamicTokenType dynamicType, std::string v, const int l): value(DynamicToken(dynamicType, std::move(v))), line(l) {}

    // [[nodiscard]] indicates that the return value should not be ignored.
    [[nodiscard]] bool isStatic() const { return std::holds_alternative<StaticToken>(value); }
    [[nodiscard]] bool isDynamic() const { return std::holds_alternative<DynamicToken>(value); }

    /* 
    * Concept: Type-based std::get_if is used to safely retrieve the value from the variant.
               std::get_if returns a pointer to the value if it holds the requested type, or nullptr otherwise.
    */ 

    // Returns the StaticToken if the token is static; otherwise, throws an error.
    [[nodiscard]] StaticToken getStatic() const {
        if (auto* st = std::get_if<StaticToken>(&value)) {
            return *st;
        }
        throw std::runtime_error("Expected static token");
    }

    // Returns the DynamicToken if the token is dynamic; otherwise, throws an error.
    [[nodiscard]] DynamicToken getDynamic() const {
        if (auto* dt = std::get_if<DynamicToken>(&value)) {
            return *dt;
        }
        throw std::runtime_error("Expected dynamic token");
    }

    // Returns the string value of a dynamic token.
    [[nodiscard]] std::string getStringValue() const {
        return getDynamic().value;
    }

    // Converts the string value of a dynamic token to an integer.
    [[nodiscard]] int getIntValue() const {
        try {
            return std::stoi(getDynamic().value);
        } catch (...) {
            throw std::runtime_error("Invalid integer constant");
        }
    }
};


#endif //LWGPP_TOKEN_H