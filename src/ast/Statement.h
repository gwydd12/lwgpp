#pragma once
#include "../token/Token.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

struct Statement {
    virtual ~Statement() = default;
};

enum class Operator {
    ADDITION, SUBTRACTION
};

inline Operator getOperator(const Token& token) {
    if (!token.isStatic()) {
        throw std::runtime_error("Token is not a static token.");
    }

    switch (std::get<StaticToken>(token.value).type) {
        case StaticTokenType::PLUS: return Operator::ADDITION;
        case StaticTokenType::MINUS: return Operator::SUBTRACTION;
        default: throw std::runtime_error("Token is not a valid operator.");
    }
}

struct Assignment final : Statement {
    std::string assignee;
    std::string variable;
    Operator op;
    int constant;
    int line;

    Assignment(std::string a, std::string v, const Operator o, const int c, const int l)
        : assignee(std::move(a)), variable(std::move(v)), op(o), constant(c), line(l) {}
};

struct Loop final : Statement {
    bool constantCondition;
    std::string variableCondition;
    std::vector<std::unique_ptr<Statement>> body;
    int line;

    Loop(const bool cc, std::string vc, std::vector<std::unique_ptr<Statement>> b, const int l)
        : constantCondition(cc), variableCondition(std::move(vc)), body(std::move(b)), line(l) {}
};

struct While final : Statement {
    std::string variable;
    int constant;
    std::vector<std::unique_ptr<Statement>> body;
    int line;

    While(std::string v, const int c, std::vector<std::unique_ptr<Statement>> b, const int l)
        : variable(std::move(v)), constant(c), body(std::move(b)), line(l) {}
};

struct Goto final : Statement {
    std::string marker;
    int markerLine;
    int line;

    Goto(std::string m, const int ml, const int l)
        : marker(std::move(m)), markerLine(ml), line(l) {}
};

struct If final : Statement {
    std::string variable;
    int constant;
    std::string marker;
    int markerLine;
    int line;

    If(std::string v, const int c, std::string m, const int ml, const int l)
        : variable(std::move(v)), constant(c), marker(std::move(m)), markerLine(ml), line(l) {}
};

struct Halt final : Statement {
    int markerLine;
    int line;

    Halt(const int ml, const int l)
        : markerLine(ml), line(l) {}
};

// Use pointer alternatives to avoid copying non-copyable members
using StatementTypes = std::variant<
    const Assignment*,
    const Loop*,
    const While*,
    const Goto*,
    const If*,
    const Halt*
>;

inline StatementTypes getStatementType(const Statement& statement) {
    if (const auto* assignment = dynamic_cast<const Assignment*>(&statement)) {
        return assignment;
    }
    if (const auto* loop = dynamic_cast<const Loop*>(&statement)) {
        return loop;
    }
    if (const auto* whileStmt = dynamic_cast<const While*>(&statement)) {
        return whileStmt;
    }
    if (const auto* gotoStmt = dynamic_cast<const Goto*>(&statement)) {
        return gotoStmt;
    }
    if (const auto* ifStmt = dynamic_cast<const If*>(&statement)) {
        return ifStmt;
    }
    if (const auto* haltStmt = dynamic_cast<const Halt*>(&statement)) {
        return haltStmt;
    }
    throw std::runtime_error("Unknown statement type at line");
}