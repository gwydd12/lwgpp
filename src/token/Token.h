#ifndef LWGPP_TOKEN_H
#define LWGPP_TOKEN_H
#include <stdexcept>
#include <string>
#include <variant>

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
    std::variant<StaticToken, DynamicToken> value;
    int line;

    Token(StaticToken staticType, const int l): value(staticType), line(l) {}
    // Using std::move to avoid unnecessary copies of the string value.
    Token(DynamicToken dynamicType, const int l): value(std::move(dynamicType)), line(l) {}
    Token(StaticTokenType staticType, const int l): value(StaticToken(staticType)), line(l) {}
    Token(DynamicTokenType dynamicType, std::string v, const int l): value(DynamicToken(dynamicType, std::move(v))), line(l) {}

    // [[nodiscard]] indicates that the return value should not be ignored.
    [[nodiscard]] bool isStatic() const { return std::holds_alternative<StaticToken>(value); }
    [[nodiscard]] bool isDynamic() const { return std::holds_alternative<DynamicToken>(value); }

    [[nodiscard]] StaticToken getStatic() const {
        if (auto* st = std::get_if<StaticToken>(&value)) {
            return *st;
        }
        throw std::runtime_error("Expected static token");
    }

    [[nodiscard]] DynamicToken getDynamic() const {
        if (auto* dt = std::get_if<DynamicToken>(&value)) {
            return *dt;
        }
        throw std::runtime_error("Expected dynamic token");
    }

    [[nodiscard]] std::string getStringValue() const {
        return getDynamic().value;
    }

    [[nodiscard]] int getIntValue() const {
        try {
            return std::stoi(getDynamic().value);
        } catch (...) {
            throw std::runtime_error("Invalid integer constant");
        }
    }
};


#endif //LWGPP_TOKEN_H