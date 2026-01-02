#include "Environment.h"
#include <stdexcept>
#include <utility>

void Environment::initVariablesIfAbsent(std::initializer_list<std::string> vars) {
    for (const auto& var : vars) {
        std::pmr::polymorphic_allocator<char> char_alloc(mem_resource_);
        PmrString key(var, char_alloc);
        if (!variables_.contains(key)) {
            variables_.emplace(std::piecewise_construct,
                               std::forward_as_tuple(key),
                               std::forward_as_tuple(0));
        }
    }
}

void Environment::setVariable(const std::string& var, int value) {
    std::pmr::polymorphic_allocator<char> char_alloc(mem_resource_);
    PmrString key(var, char_alloc);
    variables_[key] = value;
}

int Environment::getVariableValue(const std::string& var) const {
    std::pmr::polymorphic_allocator<char> char_alloc(mem_resource_);
    const PmrString key(var, char_alloc);
    const auto it = variables_.find(key);
    if (it == variables_.end()) {
        throw std::out_of_range("variable not found");
    }
    return it->second;
}

std::map<std::string, int> Environment::getVariables() const {
    std::map<std::string, int> out;
    for (const auto& kv : variables_) {
        out.emplace(std::string(kv.first), kv.second);
    }
    return out;
}

std::optional<memory::TrackingMemoryResource::Stats> Environment::getMemoryStats() const {
    if (!mem_resource_) return std::nullopt;
    const auto tracker = dynamic_cast<memory::TrackingMemoryResource*>(mem_resource_);
    if (!tracker) return std::nullopt;
    return tracker->get_stats();
}
