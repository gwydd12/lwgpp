#include "LWInterpreter.h"

void LWInterpreter::interpret(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        interpretStatement(*stmt);
    }
}

void LWInterpreter::interpretStatement(const Statement& statement) {
    StatementTypes stmtType = getStatementType(statement);
    std::visit([this](auto const & typePtr) {
        using T = std::decay_t<decltype(typePtr)>;
        if constexpr (std::is_same_v<T, const Assignment*>) {
            if (!typePtr) throw std::runtime_error("null Assignment pointer");
            LWInterpreter::interpretAssignment(*typePtr);
        } else if constexpr (std::is_same_v<T, const Loop*>) {
            if (!typePtr) throw std::runtime_error("null Loop pointer");
            this->interpretLoop(*typePtr);
        } else if constexpr (std::is_same_v<T, const While*>) {
            if (!typePtr) throw std::runtime_error("null While pointer");
            this->interpretWhile(*typePtr);
        } else {
            throw std::runtime_error("unexpected statement type");
        }
    }, stmtType);
}

void LWInterpreter::interpretLoop(const Loop& loop) {
    const bool usesConstant = loop.constantCondition;
    std::string variable = loop.variableCondition;
    int variableValue = 0;
    if (!usesConstant) {
        Environment& env = this->env();
        env.initVariablesIfAbsent({variable});
        variableValue = env.getVariableValue(variable);

    } else variableValue = std::stoi(variable);

    for (int i = 0; i < variableValue; i++) {
        interpret(loop.body);
    }
}

void LWInterpreter::interpretWhile(const While& whileStmt) {
    std::string variable = whileStmt.variable;
    const int constant = whileStmt.constant;

    Environment& env = this->env();
    env.initVariablesIfAbsent({variable});

    while (env.getVariableValue(variable) > constant) {
        interpret(whileStmt.body);
    }
}
