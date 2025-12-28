#include "Token.h"
/**
 * A Token can be either a static type or a dynamic type.
 * The struct Token uses std::variant to represent a type-safe union of the two categories.
 * (https://en.cppreference.com/w/cpp/utility/variant.html)
 **/