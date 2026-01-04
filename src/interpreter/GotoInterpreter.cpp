#include "GotoInterpreter.h"

using namespace interpreter::goto_lang;

void GotoPolicy::run(Interpreter<GotoPolicy>& self, const Statements& stmts) {
    auto& st = self.getState(); // assign state struct
    st.halted = false; // reset halted flag in state struct

    while (!st.halted) {
        if (st.pc < 0 || static_cast<size_t>(st.pc) >= stmts.size()) {
            throw std::out_of_range("Program counter out of range");
        }

        const auto& stmtPtr = stmts.at(st.pc);

        if (!stmtPtr) {
            ++st.pc;
            continue;
        }

        dispatch(self, *stmtPtr);
    }
}

int GotoPolicy::findLineWithMarker(const Interpreter<GotoPolicy>& self,
                              const std::string& marker) {
    // map stores marker -> source line, interpreter uses index => line-1
    return self.getState().markerLineMap.at(marker) - 1;
}

void GotoPolicy::dispatch(Interpreter<GotoPolicy>& self, const Statement& s) {
    StatementTypes st = getStatementType(s);

    std::visit([&](auto ptr) {
        using P = std::decay_t<decltype(ptr)>;
        if (!ptr) throw std::runtime_error("Null statement pointer in GOTO");

        auto& state = self.getState();

        if constexpr (std::is_same_v<P, const Assignment*>) {
            self.interpretAssignment(*ptr);
            ++state.pc;
        } else if constexpr (std::is_same_v<P, const If*>) {
            interpretIf(self, *ptr);
        } else if constexpr (std::is_same_v<P, const Goto*>) {
            interpretGoto(self, *ptr);
        } else if constexpr (std::is_same_v<P, const Halt*>) {
            state.halted = true;
        } else if constexpr (
            std::is_same_v<P, const Loop*> ||
            std::is_same_v<P, const While*>
        ) {
            throw std::runtime_error("LW statement found in GOTO program");
        } else {
            static_assert(dependent_false_v<P>, "Unhandled statement in GOTO");
        }
    }, st);
}

void GotoPolicy::interpretIf(Interpreter<GotoPolicy>& self, const If& ifStmt) {
    auto& st = self.getState();

    const std::string& variable = ifStmt.variable;
    const int constant = ifStmt.constant;
    const std::string& marker = ifStmt.marker;

    self.getEnvironment().initVariablesIfAbsent({variable});
    if (self.getEnvironment().getVariableValue(variable) == constant) {
        st.pc = findLineWithMarker(self, marker);
    } else ++st.pc;
}

void GotoPolicy::interpretGoto(Interpreter<GotoPolicy>& self, const Goto& gotoStmt) {
    self.getState().pc = findLineWithMarker(self, gotoStmt.marker);
}

void GotoInterpreter::setMarkerLineMap(const std::map<std::string, int>& m) {
    getState().markerLineMap = m;
}

int GotoInterpreter::findLineWithMarker(const std::string& marker) const {
    return getState().markerLineMap.at(marker) - 1;
}
