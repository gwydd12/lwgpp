//
// Created by stdin on 10/30/25.
//

#ifndef LWGPP_TOKENTYPE_H
#define LWGPP_TOKENTYPE_H
#include <stdexcept>
#include <string>
#include <variant>

/**
 * Enum classes are used for better type safety and to avoid name clashes.
 * - Also conventional enums implicitly convert to int
 * - conventional enums cause name clashes as they export to surrounding scope
 * @References
 * - https://www.stroustrup.com/C++11FAQ.html#enum
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

/**
 * A Token can be either a static type or a dynamic type.
 * The static type do not carry a value, while the dynamic type does store a string value.
 * We use std::variant to represent a type-safe union of the two categories. (https://en.cppreference.com/w/cpp/utility/variant.html
 */
struct Token {
    std::variant<StaticTokenType, DynamicToken> value;
    int line;

    Token(StaticTokenType staticType, const int l): value(staticType), line(l) {}
    // Using std::move to avoid unnecessary copies of the string value.
    Token(const DynamicTokenType dynamicType, std::string v, const int l): value(DynamicToken{dynamicType, std::move(v)}), line(l) {}

    // [[nodiscard]] indicates that the return value should not be ignored.
    [[nodiscard]] bool isStatic() const { return std::holds_alternative<StaticTokenType>(value); }
    [[nodiscard]] bool isDynamic() const { return std::holds_alternative<DynamicToken>(value); }

    std::string getValue() {
        if (auto* dynamic = std::get_if<DynamicToken>(&value)) {
            return dynamic->value;
        }

        throw std::runtime_error("Cannot get string value from static token type");
    }
};


#endif //LWGPP_TOKENTYPE_H