#ifndef LWGPP_GOTOINTERPRETER_H
#define LWGPP_GOTOINTERPRETER_H

#include "Interpreter.h"
#include <map>
#include <string>

namespace interpreter::goto_lang {
    struct GotoPolicy {
        struct State {
            int pc{0};
            bool halted{false};
            std::map<std::string, int> markerLineMap{};
        };

        static void run(Interpreter<GotoPolicy>& self, const Statements& stmts);

    private:
        static int findLineWithMarker(const Interpreter<GotoPolicy>& self,
                                      const std::string& marker);
        static void dispatch(Interpreter<GotoPolicy>& self, const Statement& s);
        static void interpretIf(Interpreter<GotoPolicy>& self, const If& ifStmt);
        static void interpretGoto(Interpreter<GotoPolicy>& self, const Goto& gotoStmt);

        friend class Interpreter<GotoPolicy>;
    };

    static void run(InterpreterT<GotoPolicy>& self, const Statements& stmts) {
        auto& st = self.state(); // assign state struct
        st.halted = false; // reset halted flag in state struct

        while (!st.halted && !self.shouldHalt()) { // check both Halt statement and interpreter halt
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

private:
    static int findLineWithMarker(const InterpreterT<GotoPolicy>& self,
                                  const std::string& marker) {
        // map stores marker -> source line, interpreter uses index => line-1
        return self.state().markerLineMap.at(marker) - 1;
    }

    static void dispatch(InterpreterT<GotoPolicy>& self, const Statement& s) {

        StatementTypes st = getStatementType(s);

        std::visit([&](auto ptr) {
            using P = std::decay_t<decltype(ptr)>;
            if (!ptr) throw std::runtime_error("Null statement pointer in GOTO");

            auto& state = self.state();

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

        void setMarkerLineMap(const std::map<std::string, int>& m);
        int findLineWithMarker(const std::string& marker) const;
    };
}

#endif // LWGPP_GOTOINTERPRETER_H
