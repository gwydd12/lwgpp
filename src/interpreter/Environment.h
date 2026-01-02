#ifndef LWGPP_ENVIRONMENT_H
#define LWGPP_ENVIRONMENT_H
#include <map>
#include <memory_resource>
#include <optional>
#include <string>
#include "../memory/TrackingMemoryResource.h"

/**
 * Represents the execution environment of the interpreter.
 * Stores and manages variables and their associated values.
 *
 * Advanced concepts:
 * - Encapsulation
 * - RAII
 * - Polymorphic memory resources
 */
class Environment {
public:
    explicit Environment(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : mem_resource_(mr), values_(mr) {}

    // Copy constructor: copy variables using the same memory resource pointer
    Environment(const Environment& other)
        : mem_resource_(other.mem_resource_),
          values_(other.values_.begin(), other.values_.end(), mem_resource_) {}

    // Copy assignment
    Environment& operator=(const Environment& other) {
        if (this == &other) return *this;
        mem_resource_ = other.mem_resource_;
        std::pmr::map<std::pmr::string, int> tmp(other.values_.begin(), other.values_.end(), mem_resource_);
        values_.swap(tmp);
        return *this;
    }

    // Move constructor
    Environment(Environment&& other) noexcept
        : mem_resource_(other.mem_resource_),
          values_(std::move(other.values_)) {
        other.mem_resource_ = std::pmr::get_default_resource();
    }

    // Move assignment
    Environment& operator=(Environment&& other) noexcept {
        if (this == &other) return *this;
        mem_resource_ = other.mem_resource_;
        values_ = std::move(other.values_);
        other.mem_resource_ = std::pmr::get_default_resource();
        return *this;
    }

    ~Environment() = default;

    void initVariablesIfAbsent(std::initializer_list<std::string> vars);
    void setVariable(const std::string& var, int value);
    int getVariableValue(const std::string& var) const;
    std::map<std::string, int> getVariables() const;
    std::optional<memory::TrackingMemoryResource::Stats> getMemoryStats() const;

private:
    std::pmr::memory_resource* mem_resource_;
    std::pmr::map<std::pmr::string, int> values_;
};

#endif // LWGPP_ENVIRONMENT_H