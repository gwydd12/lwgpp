#ifndef LWGPP_INTERPRETER_H
#define LWGPP_INTERPRETER_H
#include "Environment.h"
#include "../ast/Statement.h"

class Interpreter {
public:
    explicit Interpreter(const Environment& env) {
        environment = env;
    }
    virtual ~Interpreter() = default;
    virtual void interpret(const std::vector<std::unique_ptr<Statement>> &statements) = 0;
    static void interpretAssignment(const Assignment& assignment);

    static Environment environment;
};

#endif //LWGPP_INTERPRETER_H