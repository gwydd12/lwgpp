#include "Environment.h"
#include "../error/ErrorHandler.h"
#include "../debugger/Debugger.h"
#include <iostream>

Environment::Environment() : debugMode(false) {
    variables.clear();
    variables["x0"] = 0; // Initialize x0 with default value of 0
}

Environment::Environment(bool debugMode) : debugMode(debugMode) {
    variables.clear();
    variables["x0"] = 0; // Initialize x0 with default value of 0
}

Environment::Environment(const std::unordered_map<std::string, int>& variables, bool debugMode) 
    : debugMode(debugMode) {
    this->variables = variables;
}

void Environment::initVariableIfAbsent(const std::string& variable) {
    if (variables.find(variable) == variables.end()) {
        variables[variable] = 0;
    }
}

void Environment::initVariablesIfAbsent(const std::initializer_list<std::string>& vars) {
    for (const auto& var : vars) {
        initVariableIfAbsent(var);
    }
}

void Environment::setVariable(int line, const std::string& variable, int value) {
    if (value < 0) {
        ErrorHandler::report(0, "Variable value cannot be negative");
    }
    variables[variable] = value;
    addDebugVariablesInDebugMode(line);
}

void Environment::setVariable(const std::string& variable, int value) {
    if (value < 0) {
        ErrorHandler::report(0, "Variable value cannot be negative");
    }
    variables[variable] = value;
}

int Environment::getVariable(const std::string& variable) {
    initVariableIfAbsent(variable);
    return variables[variable];
}

std::unordered_map<std::string, int> Environment::getVariables() const {
    return variables;
}

void Environment::setVariables(const std::unordered_map<std::string, int>& vars) {
    for (const auto& entry : vars) {
        setVariable(0, entry.first, entry.second);
    }
}

void Environment::addDebugVariablesInDebugMode(int line) {
    initVariableIfAbsent("x0"); // Ensure x0 exists before adding to debug
    if (debugMode) {
        Debugger::addDebugVariable(line, variables);
        for (const auto& entry : variables) {
            std::cerr << entry.first << "=" << entry.second << " ";
        }
        std::cerr << std::endl;
    }
}