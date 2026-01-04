#ifndef LWGPP_LWINTERPRETER_H
#define LWGPP_LWINTERPRETER_H

#include "Interpreter.h"
#include <string>

namespace lwgpp::interp {

// ---------- LW Policy ----------
struct LWPolicy {
    struct State { /* no program counter needed */ };

    static void run(Interpreter<LWPolicy>& self, const Statements& stmts) {
        for (const auto& ptr : stmts) {
            if (!ptr) continue; // LW shouldn't have nulls, but safe
            dispatch(self, *ptr);
        }
    }

private:
    static void dispatch(Interpreter<LWPolicy>& self, const Statement& s) {
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

    static void interpretLoop(Interpreter<LWPolicy>& self, const Loop& loop) {
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
            self.interpret(loop.body);
        }
    }

    static void interpretWhile(Interpreter<LWPolicy>& self, const While& w) {
        const std::string& var = w.variable;
        const int constant = w.constant;

        self.environment().initVariablesIfAbsent({var});

        while (self.environment().getVariableValue(var) > constant) {
            self.interpret(w.body);
        }
    }
};

class LWInterpreter final : public Interpreter<LWPolicy> {
public:
    using Interpreter::Interpreter;
};

} // namespace lwgpp::interp

#endif // LWGPP_LWINTERPRETER_H
