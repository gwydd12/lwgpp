#include "Interpreter.h"

#include "../error/ErrorHandler.h"
#include "Assignment.h"
#include "Statement.h"
#include "Operator.h"

#include <algorithm>
#include <limits>
#include <iostream>

Interpreter::Interpreter(Environment& env) : environment(env) {}

std::future<void> Interpreter::interpretAsync(const std::vector<Statement*>& statements) {
    return std::async(std::launch::async, [this, &statements]() {
        this->interpret(statements);
    });
}

void Interpreter::halt() {
    isHalted.store(true);
    // wake any scheduled waiters
    schedulerCv.notify_all();
}

void Interpreter::setHaltTimeout(int seconds) {
    std::lock_guard<std::mutex> lg(schedulerMutex);
    haltTimeout = seconds;
}

bool Interpreter::is_halted() const {
    return isHalted.load();
}

void Interpreter::scheduleHalt() {
    // spawn a detached thread that waits for haltTimeout seconds or until notified
    std::thread([this]() {
        std::unique_lock<std::mutex> lk(this->schedulerMutex);
        if (this->schedulerCv.wait_for(lk, std::chrono::seconds(this->haltTimeout)) == std::cv_status::timeout) {
            // time expired -> halt
            this->halt();
        }
    }).detach();
}

void Interpreter::interpretAssignment(const Assignment& assignment) {
    int line = assignment.line();
    int firstVariable = assignment.variable1Number();
    int secondVariable = assignment.variable2Number();
    Operator op = assignment.operatorType();
    int constant = assignment.constant();

    environment.initVariablesIfAbsent(firstVariable, secondVariable);
    int secondValue = environment.getVariable(secondVariable);

    // perform checked addition to detect overflow
    try {
        if (op == Operator::ADDITION) {
            long long sum = static_cast<long long>(secondValue) + static_cast<long long>(constant);
            if (sum > std::numeric_limits<int>::max() || sum < std::numeric_limits<int>::min()) {
                throw std::overflow_error("integer overflow");
            }
            environment.setVariable(line, firstVariable, static_cast<int>(sum));
        } else if (op == Operator::SUBTRACTION) {
            long long diff = static_cast<long long>(secondValue) - static_cast<long long>(constant);
            int result = static_cast<int>(std::max(0LL, diff));
            environment.setVariable(line, firstVariable, result);
        } else {
            ErrorHandler::report(line, "Error in Interpreter::interpretAssignment: Unknown operator type");
        }
    } catch (const std::overflow_error&) {
        ErrorHandler::report(line, "There was an arithmetic overflow.");
        environment.setVariable(line, firstVariable, 0);
    }
}