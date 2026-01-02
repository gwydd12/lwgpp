#include "Interpreter.h"
#include "Environment.h"

void Interpreter::interpretAssignment(const Assignment& assignment) const {
    const int line = assignment.line;
    std::string assignee = assignment.assignee;
    std::string variable = assignment.variable;
    const Operator op = assignment.op;
    const int constant = assignment.constant;

    Environment& env = this->env();
    env.initVariablesIfAbsent({assignee, variable});
    const int variableValue = env.getVariableValue(variable);
    try {
        switch (op) {
            case Operator::ADDITION: env.setVariable(assignee, variableValue + constant); break;
            case Operator::SUBTRACTION: env.setVariable(assignee, variableValue - constant); break;
        }
    } catch (std::exception& e) {
        throw std::runtime_error("Runtime error at line " + std::to_string(line) + ": " + e.what());
    }
}