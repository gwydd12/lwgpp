#ifndef LWGPP_ENVIRONMENT_H
#define LWGPP_ENVIRONMENT_H
#include <map>
#include <memory_resource>
#include <optional>
#include <string>
#include <unordered_map>
#include "../memory/TrackingMemoryResource.h"

/**
 * Represents the execution environment of the interpreter.
 * Stores and manages variables and their associated values.
 *
 * Advanced concepts:
 * - Encapsulation
 * - RAII
 * - Standard associative containers
 */
class Environment {
public:
    explicit Environment(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : mem_resource_(mr),
          variables_(0, std::hash<PmrString>(), std::equal_to(), AllocPair(mem_resource_)) {}

    // Copy constructor: copy variables using the same memory resource pointer
    Environment(const Environment& other)
        : mem_resource_(other.mem_resource_),
          variables_(0, std::hash<PmrString>(), std::equal_to<>(), AllocPair(mem_resource_)) {
        variables_.reserve(other.variables_.size());
        for (const auto& kv : other.variables_) variables_.emplace(kv.first, kv.second);
    }

    // Copy assignment
    Environment& operator=(const Environment& other) {
        if (this == &other) return *this;
        mem_resource_ = other.mem_resource_;
        MapType tmp(0, std::hash<PmrString>(), std::equal_to<>(), AllocPair(mem_resource_));
        tmp.reserve(other.variables_.size());
        for (const auto& kv : other.variables_) tmp.emplace(kv.first, kv.second);
        variables_.swap(tmp);
        return *this;
    }

    // Move constructor
    Environment(Environment&& other) noexcept
        : mem_resource_(other.mem_resource_),
          variables_(std::move(other.variables_)) {
        other.mem_resource_ = std::pmr::get_default_resource();
    }

    // Move assignment
    Environment& operator=(Environment&& other) noexcept {
        if (this == &other) return *this;
        mem_resource_ = other.mem_resource_;
        variables_ = std::move(other.variables_);
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
    using PmrString = std::pmr::string;
    using AllocPair = std::pmr::polymorphic_allocator<std::pair<const PmrString, int>>;
    using MapType = std::unordered_map<PmrString, int, std::hash<PmrString>, std::equal_to<>, AllocPair>;

    std::pmr::memory_resource* mem_resource_;
    MapType variables_;
};

#endif // LWGPP_ENVIRONMENT_H
