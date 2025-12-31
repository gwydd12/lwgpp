#include "Token.h"
/**
 * A Token can be either a static type or a dynamic type.
 * The struct Token uses std::variant to represent a type-safe union of the two categories.
 * (https://en.cppreference.com/w/cpp/utility/variant.html)
 **/


/**
 * Retrieves the stored type (StaticToken or DynamicToken) from the variant.
 *
 * @tparam T Type to retrieve (StaticToken or DynamicToken)
 * @return Reference to the stored type
 */
template <TokenVariant Variant>
const Variant& Token::get() const {
    if (auto* ptr = std::get_if<Variant>(&value)) {
        return *ptr;
    }

    throw std::runtime_error("Type mismatch when accessing token value");
}

const std::string& Token::getStringValue() const {
    return get<DynamicToken>().value;
}

int Token::getIntValue() const {
    try {
        return std::stoi(get<DynamicToken>().value);
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid integer conversion: " + get<DynamicToken>().value);
    }
}