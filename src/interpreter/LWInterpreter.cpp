(#include "LWInterpreter.h")

#include "../error/ErrorHandler.h"
#include "Assignment.h"
#include "Loop.h"
#include "While.h"
#include "Statement.h"

#include <algorithm>
#include <exception>

LWInterpreter::LWInterpreter(Environment& env) : Interpreter(env) {} // call base constructor

void LWInterpreter::interpret(const std::vector<Statement*>& statements) {
	if (&statements == nullptr) {
		ErrorHandler::report(0, "Error in Interpreter.interpret: statements must not be null");
		return;
	}

	try {
		for (const Statement* stmtPtr : statements) {
			if (is_halted()) break;
			if (stmtPtr == nullptr) continue;
			interpretStatement(*stmtPtr);
		}
	} catch (const std::exception& e) {
		ErrorHandler::report(0, std::string("Unexpected runtime exception:") + e.what());
	}
}

std::future<void> LWInterpreter::interpretAsync(const std::vector<Statement*>& statements) {
	scheduleHalt();
	return std::async(std::launch::async, [this, &statements]() {
		this->interpret(statements);
		if (this->is_halted()) {
			ErrorHandler::report(0, "Execution got halted, possibly due to an infinite loop");
		}
	});
}

void LWInterpreter::interpretStatement(const Statement& statement) {
	// Try dynamic cast to known statement types
	if (const Assignment* a = dynamic_cast<const Assignment*>(&statement)) {
		// call base class helper
		interpretAssignment(*a);
		return;
	}
	if (const Loop* l = dynamic_cast<const Loop*>(&statement)) {
		interpretLoop(*l);
		return;
	}
	if (const While* w = dynamic_cast<const While*>(&statement)) {
		interpretWhile(*w);
		return;
	}

	ErrorHandler::report(0, "Error in Interpreter.interpretStatement: Unknown statement type");
}

void LWInterpreter::interpretLoop(const Loop& loop) {
	int line = loop.line();
	const std::vector<Statement*>& stmts = loop.statements();
	bool usesConstant = loop.usesConstant();
	int number = loop.number();

	if (!usesConstant) {
		environment.initVariablesIfAbsent(number);
		number = environment.getVariable(number);
	}

	for (int i = 0; i < number; ++i) {
		environment.addDebugVariablesInDebugMode(line);
		if (is_halted()) break;
		interpret(stmts);
	}
}

void LWInterpreter::interpretWhile(const While& whileStatement) {
	int line = whileStatement.line();
	const std::vector<Statement*>& stmts = whileStatement.statements();
	int variable = whileStatement.variableNumber();
	int constant = whileStatement.constant();

	environment.initVariablesIfAbsent(variable);

	while (environment.getVariable(variable) > constant) {
		environment.addDebugVariablesInDebugMode(line);
		if (is_halted()) break;
		interpret(stmts);
	}
}



LWInterpreter::LWInterpreter(Environment& env) : Interpreter(env) {}

