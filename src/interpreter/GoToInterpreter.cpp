#include "GoToInterpreter.h"
#include "../error/ErrorHandler.h"
#include "Assignment.h"
#include "If.h"
#include "Goto.h"
#include "Halt.h"
#include "Statement.h"
#include "Environment.h"

#include <algorithm>
#include <exception>

GoToInterpreter::GoToInterpreter(Environment& env) 
    : Interpreter(env), programCounter(0), statementList({}) {}

void GoToInterpreter::setMarkerLineMap(const std::map<int, int>& markerLineMap) {
    if (markerLineMap.empty()) {
        ErrorHandler::report(0, "Marker line map must not be null or empty");
        return;
    }
    this->markerLineMap = markerLineMap;
}

void GoToInterpreter::interpret(const std::vector<Statement*>& statements) {
    if (statements.empty()) {
        ErrorHandler::report(0, "Statements must not be null or empty");
        return;
    }

    this->statementList = statements;
    programCounter = 0;

    try {
        executeStatements();
    } catch (const std::exception& e) {
        ErrorHandler::report(0, std::string("Unexpected runtime exception:") + e.what());
    }
}

std::future<void> GoToInterpreter::interpretAsync(const std::vector<Statement*>& statements) {
    scheduleHalt();
    return std::async(std::launch::async, [this, &statements]() {
        this->interpret(statements);
        if (this->is_halted()) {
            ErrorHandler::report(0, "Execution got halted, possibly due to an infinite loop");
        }
    });
}

void GoToInterpreter::executeStatements() {
    try {
        while (!is_halted()) {
            // Check bounds first
            if (programCounter < 0 || programCounter >= static_cast<int>(statementList.size())) {
                break;
            }

            Statement* currentStatement = statementList[programCounter];
            
            // Try dynamic cast to Halt
            if (const Halt* haltPtr = dynamic_cast<const Halt*>(currentStatement)) {
                environment.addDebugVariablesInDebugMode(haltPtr->line());
                break;
            }

            if (currentStatement == nullptr) {
                programCounter++;
            } else {
                interpretStatement(*currentStatement);
            }
        }
    } catch (const std::exception& e) {
        ErrorHandler::report(0, std::string("Unexpected runtime exception:") + e.what());
    }
}

void GoToInterpreter::interpretStatement(const Statement& statement) {
    // Try dynamic cast to known statement types
    if (const Assignment* a = dynamic_cast<const Assignment*>(&statement)) {
        interpretAssignment(*a);
        programCounter++;
        return;
    }
    
    if (const If* ifStmt = dynamic_cast<const If*>(&statement)) {
        interpretIf(*ifStmt);
        return;
    }
    
    if (const Goto* gotoStmt = dynamic_cast<const Goto*>(&statement)) {
        interpretGoto(*gotoStmt);
        return;
    }

    ErrorHandler::report(0, "Error in Interpreter.interpretStatement: Unknown statement type");
}

void GoToInterpreter::interpretIf(const If& ifStatement) {
    int line = ifStatement.line();
    int variable = ifStatement.variableNumber();
    int constant = ifStatement.constant();
    int marker = ifStatement.gotoMarkerNumber();

    environment.addDebugVariablesInDebugMode(line);
    environment.initVariablesIfAbsent({ifStatement.getVariableName(variable)});
    int value = environment.getVariable(ifStatement.getVariableName(variable));

    if (value == constant) {
        programCounter = findLineWithMarker(marker);
    } else {
        programCounter++;
    }
}

void GoToInterpreter::interpretGoto(const Goto& gotoStatement) {
    int line = gotoStatement.line();
    environment.addDebugVariablesInDebugMode(line);
    programCounter = findLineWithMarker(gotoStatement.markerNumber());
}

int GoToInterpreter::findLineWithMarker(int marker) {
    auto it = markerLineMap.find(marker);
    if (it != markerLineMap.end()) {
        return it->second - 1; // Convert to zero-based index
    }
    ErrorHandler::report(0, "Marker " + std::to_string(marker) + " not found in marker map");
    return programCounter; // Stay at current position if marker not found
}
