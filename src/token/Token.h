#ifndef LWGPP_TOKEN_H
#define LWGPP_TOKEN_H
#include <stdexcept>
#include <string>
#include <variant>
#include <concepts>

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

    /**
     * Perfect forwarding constructor for DynamicToken.
     * Allows to keep the value category (lvalue/rvalue) of the argument.
     * Depending on the value category of 'v', it will either copy or move the string into the member variable.
     * std::forward is the same as std::static_cast<T&&>(v), preserving the value category.
     *
     * TODO: Maybe call this somewhere in the codebase as a showcase?
     */
    template <typename T>
    DynamicToken(const DynamicTokenType t, T&& v): type(t), value(std::forward<T>(v)) {}
    DynamicToken(const DynamicTokenType t, std::string v): type(t), value(std::move(v)) {}
};

struct StaticToken {
    StaticTokenType type;

    explicit StaticToken(const StaticTokenType t): type(t) {}
};

/**
 * Metaprogramming struct to map token categories to their names and types.
 * We have two specializations: one for StaticTokenType and one for DynamicTokenType.
 * This allows us to retrieve the corresponding token type and name based on the category.
 * Those traits are used in the Token struct for type-safe access at runtime in the is and getType methods.
 * Advanced concepts:
 * - Template specialization (Full specialization)
 * - Metaprogramming with traits
 * - Fixed traits (cannot be overriden)
 */
template <typename>
struct TokenTypeTraits;

template <>
struct TokenTypeTraits<StaticTokenType> {
    static constexpr auto name = "StaticToken";
    using token_t = StaticToken;
};

template <>
struct TokenTypeTraits<DynamicTokenType> {
    static constexpr auto name = "DynamicToken";
    using token_t = DynamicToken;
};

template <typename T>
concept TokenCategory =
    std::same_as<T, StaticTokenType> ||
    std::same_as<T, DynamicTokenType>;

template <typename T>
concept TokenVariant = std::same_as<T, StaticToken> || std::same_as<T, DynamicToken>;
/*
 * A Token can be either a static type or a dynamic type.
 * The static type does not carry a value, while the dynamic type does store a string value.
 * We use std::variant to represent a type-safe union of the two categories. (https://en.cppreference.com/w/cpp/utility/variant.html
 * It allows us to store either a StaticToken or a DynamicToken in a single Token instance safely.
 * Additionally, we can differentiate between the two types at runtime using std::holds_alternative and std::get_if.
 */
struct Token {
    std::variant<StaticToken, DynamicToken> value;
    int line;

    Token(const StaticTokenType t, const int l) : value(StaticToken(t)), line(l) {}
    Token(const DynamicTokenType t, std::string v, const int l) : value(DynamicToken(t, std::move(v))), line(l) {}
    Token(StaticToken t, const int l) : value(t), line(l) {}
    Token(DynamicToken t, const int l) : value(std::move(t)), line(l) {}

    template <TokenVariant Variant>
    [[nodiscard]] bool is() const {
        return std::holds_alternative<Variant>(value);
    }
    template <TokenCategory Category, Category T>
    [[nodiscard]] bool is() const {
        using token_t = TokenTypeTraits<Category>::token_t;

        if (auto* ptr = std::get_if<token_t>(&value)) {
            return ptr->type == T;
        }
        return false;
    }
    template <TokenVariant Variant>
    const Variant& get() const;
    template <TokenCategory Category>
    Category getType() const {
        using token_t = TokenTypeTraits<Category>::token_t;

        if (auto* ptr = std::get_if<token_t>(&value)) {
            return ptr->type;
        }

        throw std::runtime_error(std::string("Token does not hold ") + TokenTypeTraits<Category>::name);
    }
    [[nodiscard]] const std::string& getStringValue() const;
    [[nodiscard]] int getIntValue() const;
};


#endif //LWGPP_TOKEN_H