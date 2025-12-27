#include "Interpreter.h"
#include "Environment.h"

Environment Interpreter::environment;

void Interpreter::interpretAssignment(const Assignment& assignment) {
    const int line = assignment.line;
    std::string assignee = assignment.assignee;
    std::string variable = assignment.variable;
    const Operator op = assignment.op;
    const int constant = assignment.constant;

    environment.initVariablesIfAbsent({assignee, variable});
    const int variableValue = environment.getVariableValue(variable);
    try {
        switch (op) {
            case Operator::ADDITION: environment.setVariable(assignee, variableValue + constant); break;
            case Operator::SUBTRACTION: environment.setVariable(assignee, variableValue - constant); break;
        }
    } catch (std::exception& e) {
        throw std::runtime_error("Runtime error at line " + std::to_string(line) + ": " + e.what());
    }
}