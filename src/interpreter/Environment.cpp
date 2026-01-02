#include "Environment.h"
#include <stdexcept>

void Environment::initVariablesIfAbsent(const std::initializer_list<std::string> vars) {
    for (const auto& var : vars) {
        if (std::pmr::string pmrKey(var, values_.get_allocator()); !values_.contains(pmrKey)) {
            values_[pmrKey] = 0;
        }
    }
}

void Environment::setVariable(const std::string& var, const int value) {
    const std::pmr::string pmrKey(var, values_.get_allocator());
    values_[pmrKey] = value;
}

int Environment::getVariableValue(const std::string& var) const {
    const std::pmr::string pmrKey(var, values_.get_allocator());
    const auto it = values_.find(pmrKey);
    if (it == values_.end()) {
        throw std::out_of_range("variable not found");
    }
    return it->second;
}

std::map<std::string, int> Environment::getVariables() const {
    std::map<std::string, int> out;
    for (const auto& [pmrKey, value] : values_) {
        out.emplace(std::string(pmrKey), value);
    }
    return out;
}

std::optional<memory::TrackingMemoryResource::Stats> Environment::getMemoryStats() const {
    const auto tracker = dynamic_cast<memory::TrackingMemoryResource*>(values_.get_allocator().resource());
    if (!tracker) return std::nullopt;
    return tracker->get_stats();
}