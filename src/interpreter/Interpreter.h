#ifndef LWGPP_INTERPRETER_H
#define LWGPP_INTERPRETER_H

#include "Environment.h"
#include "../ast/Statement.h"
#include <type_traits>
#include <variant>
#include <vector>
#include <memory>
#include <stdexcept>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

namespace lwgpp::interp {

// ---------- tiny metaprogramming helper ----------
template<class>
inline constexpr bool dependent_false_v = false; // makes error dependent on template parameter

// Alias
using Statements = std::vector<std::unique_ptr<Statement>>;

// ---------- Policy-based Interpreter ----------
template<class Policy>
class InterpreterT {
public:
    using policy_type = Policy;
    using state_type  = typename Policy::State;

    void interpret(const Statements& stmts) {
        Policy::run(*this, stmts);
    }

    void interpretAsync(const Statements& stmts) {
        halted_ = false;
        worker_ = std::thread([this, &stmts]() {
            Policy::run(*this, stmts);
            halted_ = true;
        });
    }

    void join() {
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    void halt() {
        halted_.store(true,  std::memory_order_relaxed);
    }

    bool isHalted() const {
        return halted_;
    }

    bool shouldHalt() const {
        return halted_.load(std::memory_order_relaxed);
    }

    virtual ~InterpreterT() = default;

    // Own environment by value
    explicit InterpreterT(Environment env = {})
        : environment_(std::move(env)) {}

    // void interpret(const Statements& stmts) {
    //     Policy::run(*this, stmts);
    // }

    // Access to environment 
    Environment& environment() {
        return environment_; 
    }
    const Environment& environment() const { return environment_; }

    // Shared piece: assignment execution
    void interpretAssignment(const Assignment& assignment) {

        const int line = assignment.line;
        const std::string& assignee = assignment.assignee;
        const std::string& variable = assignment.variable;
        const Operator op = assignment.op;
        const int constant = assignment.constant;

        environment_.initVariablesIfAbsent({assignee, variable});
        const int variableValue = environment_.getVariableValue(variable);

        try {
            switch (op) {
                case Operator::ADDITION:
                    environment_.setVariable(assignee, variableValue + constant);
                    break;
                case Operator::SUBTRACTION:
                    environment_.setVariable(assignee, variableValue - constant);
                    break;
                default:
                    throw std::runtime_error("Unknown operator");
            }
        } catch (const std::exception& e) {
            throw std::runtime_error(
                "Runtime error at line " + std::to_string(line) + ": " + e.what()
            );
        }
    }

    // Policy gets access to state via these
    state_type& state() { return state_; }
    const state_type& state() const { return state_; }

private:
    Environment environment_;
    state_type state_{};
    std::atomic<bool> halted_{false};
    std::thread worker_;

    friend Policy; // lets Policy call internal helpers if needed
};

} // namespace lwgpp::interp

#endif // INTERPRETER_H
