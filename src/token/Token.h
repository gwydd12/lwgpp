#ifndef LWGPP_TOKEN_H
#define LWGPP_TOKEN_H
#include <stdexcept>
#include <string>
#include <variant>

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

/**
 * A Token can be either a static type or a dynamic type.
 * The static type does not carry a value, while the dynamic type does store a string value.
 * We use std::variant to represent a type-safe union of the two categories. (https://en.cppreference.com/w/cpp/utility/variant.html
 */
struct Token {
    std::variant<StaticToken, DynamicToken> value;
    int line;

    Token(StaticToken staticType, const int l): value(staticType), line(l) {}
    Token(DynamicToken dynamicType, const int l): value(std::move(dynamicType)), line(l) {}
    Token(StaticTokenType staticType, const int l): value(StaticToken(staticType)), line(l) {}
    Token(DynamicTokenType dynamicType, std::string v, const int l): value(DynamicToken(dynamicType, std::move(v))), line(l) {}

    template <typename T>
    [[nodiscard]] bool is() const {
        return std::holds_alternative<T>(value);
    }

    template <typename TokenCategory, TokenCategory T>
    [[nodiscard]] bool is() const {
        using token_t = typename TokenTypeTraits<TokenCategory>::token_t;

        if (auto* ptr = std::get_if<token_t>(&value)) {
            return ptr->type == T;
        }
        return false;
    }

    /**
     * Retrieves the stored type (StaticToken or DynamicToken) from the variant.
     *
     * @tparam T Type to retrieve (StaticToken or DynamicToken)
     * @return Reference to the stored type
     */
    template <typename T>
    [[nodiscard]] const T& get() const {
        if (auto* ptr = std::get_if<T>(&value)) {
            return *ptr;
        }

        throw std::runtime_error("Type mismatch when accessing token value");
    }

    template <typename T>
    [[nodiscard]] T getType() const {
        using token_t = typename TokenTypeTraits<T>::token_t;

        if (auto* ptr = std::get_if<token_t>(&value)) {
            return ptr->type;
        }

        throw std::runtime_error(std::string("Token does not hold ") + TokenTypeTraits<T>::name);
    }


    [[nodiscard]] const std::string& getStringValue() const {
        return get<DynamicToken>().value;
    }

    [[nodiscard]] int getIntValue() const {
        try {
            return std::stoi(get<DynamicToken>().value);
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid integer conversion: " + get<DynamicToken>().value);
        }
    }
};


#endif //LWGPP_TOKEN_H