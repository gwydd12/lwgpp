#ifndef LWGPP_LWINTERPRETER_H
#define LWGPP_LWINTERPRETER_H

#include "Interpreter.h"
namespace interpreter::lw {
    struct LwPolicy {
        struct State {};
        static void run(Interpreter<LwPolicy>& self, const Statements& stmts);

    private:
        static void dispatch(Interpreter<LwPolicy>& self, const Statement& s);
        static void interpretLoop(Interpreter<LwPolicy>& self, const Loop& loop);
        static void interpretWhile(Interpreter<LwPolicy>& self, const While& w);
    };

    class LWInterpreter final : public Interpreter<LwPolicy> {
    public:
        using Interpreter::Interpreter;
    };
}

#endif // LWGPP_LWINTERPRETER_H
