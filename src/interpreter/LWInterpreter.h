#ifndef LWGPP_LWINTERPRETER_H
#define LWGPP_LWINTERPRETER_H

#include "Interpreter.h"
#include <string>

namespace lwgpp::interp {

// ---------- LW Policy ----------
struct LWPolicy {
    struct State { /* no program counter needed */ };

    static void run(InterpreterT<LWPolicy>& self, const Statements& stmts) { 
        for (const auto& uptr : stmts) {
            if (!self.shouldHalt()) return; // check for halt signal 
            if (!uptr) continue; // LW shouldn't have nulls, but safe
            dispatch(self, *uptr);
        }
    }

private:
    static void dispatch(InterpreterT<LWPolicy>& self, const Statement& s) {

        if (self.shouldHalt()) return; // add a halt check again in cases
        StatementTypes st = getStatementType(s);

        std::visit([&](auto ptr) {
            using P = std::decay_t<decltype(ptr)>;
            if (!ptr) throw std::runtime_error("Null statement pointer in LW");

            if constexpr (std::is_same_v<P, const Assignment*>) {
                self.interpretAssignment(*ptr);
            } else if constexpr (std::is_same_v<P, const Loop*>) {
                interpretLoop(self, *ptr);
            } else if constexpr (std::is_same_v<P, const While*>) {
                interpretWhile(self, *ptr);
            } else if constexpr (
                std::is_same_v<P, const Goto*> ||
                std::is_same_v<P, const If*>   ||
                std::is_same_v<P, const Halt*>
            ) {
                throw std::runtime_error("GOTO statement found in LW program");
            } else {
                static_assert(dependent_false_v<P>, "Unhandled statement in LW");
            }
        }, st);
    }

    static void interpretLoop(InterpreterT<LWPolicy>& self, const Loop& loop) {
        const bool usesConstant = loop.constantCondition;
        int count = 0;

        if (!usesConstant) {
            const std::string& var = loop.variableCondition;
            self.environment().initVariablesIfAbsent({var});
            count = self.environment().getVariableValue(var);
        } else {
            count = std::stoi(loop.variableCondition);
        }

        for (int i = 0; i < count; ++i) {
            if (self.shouldHalt()) return; // add a halt check for each iteration
            self.interpretAsync(loop.body);
        }
    }

    static void interpretWhile(InterpreterT<LWPolicy>& self, const While& w) {
        const std::string& var = w.variable;
        const int constant = w.constant;

        self.environment().initVariablesIfAbsent({var});

        while (!self.shouldHalt() &&self.environment().getVariableValue(var) > constant) {
            self.interpretAsync(w.body);
        }
    }
};

class LWInterpreter final : public InterpreterT<LWPolicy> {
public:
    using InterpreterT<LWPolicy>::InterpreterT; // inherit constructors
};

} // namespace lwgpp::interp

#endif // LWGPP_LWINTERPRETER_H
