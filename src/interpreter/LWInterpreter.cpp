#include "LWInterpreter.h"

using namespace interpreter::lw;

void LwPolicy::run(Interpreter<LwPolicy>& self, const Statements& stmts) {
    for (const auto& ptr : stmts) {
        if (self.shouldHalt()) return;
        if (!ptr) continue;
        dispatch(self, *ptr);
    }
}

void LwPolicy::dispatch(Interpreter<LwPolicy>& self, const Statement& s) {
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

void LwPolicy::interpretLoop(Interpreter<LwPolicy>& self, const Loop& loop) {
    const bool usesConstant = loop.constantCondition;
    int count = 0;

    if (!usesConstant) {
        const std::string& var = loop.variableCondition;
        self.getEnvironment().initVariablesIfAbsent({var});
        count = self.getEnvironment().getVariableValue(var);
    } else {
        count = std::stoi(loop.variableCondition);
    }

    for (int i = 0; i < count; ++i) {
        if (self.shouldHalt()) return; // add a halt check for each iteration
        self.interpret(loop.body);
    }
}

void LwPolicy::interpretWhile(Interpreter<LwPolicy>& self, const While& w) {
    const std::string& var = w.variable;
    const int constant = w.constant;

    self.getEnvironment().initVariablesIfAbsent({var});

    while (!self.shouldHalt() &&self.getEnvironment().getVariableValue(var) > constant) {
        self.interpret(w.body);
    }
}