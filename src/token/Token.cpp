#include "Token.h"

const std::string& Token::getStringValue() const {
    return get<DynamicToken>().value;
}

int Token::getIntValue() const {
    try {
        return std::stoi(get<DynamicToken>().value);
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid integer conversion: " + get<DynamicToken>().value);
    }
}