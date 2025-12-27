#include "Environment.h"

void Environment::initVariablesIfAbsent(std::initializer_list<std::string> vars) {
    for (const auto& var : vars) {
        initVariableIfAbsent(var);
    }
}

void Environment::setVariable(const std::string &var, const int value) {
    variables_[var] = value;
}

int Environment::getVariableValue(const std::string& var) const {
    return variables_.at(var);
}

std::map<std::string, int> Environment::getVariables() {
    return variables_;
}
