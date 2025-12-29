#ifndef LWGPP_LWINTERPRETER_H
#define LWGPP_LWINTERPRETER_H
#include "Interpreter.h"


class LWInterpreter: public Interpreter {
public:
    explicit LWInterpreter(const Environment& env) : Interpreter(env) {}

    ~LWInterpreter() override = default;
    void interpret(const std::vector<std::unique_ptr<Statement>> &statements) override;
    void interpretStatement(const Statement& statement);
    void interpretLoop(const Loop& loop);
    void interpretWhile(const While& whileStmt);
};


#endif //LWGPP_LWINTERPRETER_H