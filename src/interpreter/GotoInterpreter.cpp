#include "GotoInterpreter.h"

void GotoInterpreter::interpret(const std::vector<std::unique_ptr<Statement> > &stmts) {
    executeStatements(stmts);
}

void GotoInterpreter::executeStatements(const std::vector<std::unique_ptr<Statement>> &stmts) {
    while (!isHalted_) {
        if (pc_ < 0 || static_cast<size_t>(pc_) >= stmts.size()) {
            throw std::out_of_range("Program counter out of range");
        }

        const auto &stmtPtr = stmts.at(pc_);
        if (!stmtPtr) {
            pc_++;
            continue;
        }

        const Statement &statement = *stmtPtr;
        interpretStatement(statement);
    }
}

void GotoInterpreter::interpretStatement(const Statement& statement) {
    StatementTypes stmtType = getStatementType(statement);
    std::visit([this](auto const & typePtr) {
        using T = std::decay_t<decltype(typePtr)>;
        if constexpr (std::is_same_v<T, const Assignment*>) {
            GotoInterpreter::interpretAssignment(*typePtr);
            pc_++;
        } else if constexpr (std::is_same_v<T, const If*>) {
            interpretIf(*typePtr);
        } else if constexpr (std::is_same_v<T, const Goto*>) {
            interpretGoto(*typePtr);
        } else if constexpr (std::is_same_v<T, const Halt*>) {
            isHalted_ = true;
        } else {
            throw std::runtime_error("IDK what happend!");
        }
    }, stmtType);
}

void GotoInterpreter::interpretIf(const If& ifStmt) {
    std::string variable = ifStmt.variable;
    const int constant = ifStmt.constant;
    const std::string marker = ifStmt.marker;

    environment.initVariablesIfAbsent({variable});
    int variableValue = environment.getVariableValue(variable);

    if (variableValue == constant) {
        pc_ = findLineWithMarker(marker);
    } else pc_++;
}

void GotoInterpreter::interpretGoto(const Goto& gotoStmt) {
    pc_ = findLineWithMarker(gotoStmt.marker);
}

int GotoInterpreter::findLineWithMarker(const std::string &marker) const {
    return markerLineMap_.at(marker) - 1;
}
