
#pragma once
#include "Interpreter.h"
#include "Statement.h"
#include "Loop.h"
#include "While.h"



class LWInterpreter : public Interpreter { 
    public:
        explicit LWInterpreter(Environment& env);
        void interpret(const std::vector<Statement*>& statements) override;
        std::future<void> interpretAsync(const std::vector<Statement*>& statements) override;

    private:
        void interpretStatement(const Statement& statement);
        void interpretLoop(const Loop& statement);
        void interpretWhile(const While& statement);
};