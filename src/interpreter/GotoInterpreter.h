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

    class GotoInterpreter final : public Interpreter<GotoPolicy> {
    public:
        using Interpreter::Interpreter;

        void setMarkerLineMap(const std::map<std::string, int>& m);
        int findLineWithMarker(const std::string& marker) const;
    };
}

#endif // LWGPP_GOTOINTERPRETER_H
