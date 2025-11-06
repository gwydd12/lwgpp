#ifndef LWGPP_STATEMENT_H
#define LWGPP_STATEMENT_H
#include <memory>
#include <string>
#include <vector>


struct Statement {
    virtual ~Statement() = default;
};

enum class Operator {
    ADDITION, SUBTRACTION
};

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
    int variable;
    int constant;
    std::vector<std::unique_ptr<Statement>> body;
    int line;

    While(const int v, const int c, std::vector<std::unique_ptr<Statement>> b, const int l)
        : variable(v), constant(c), body(std::move(b)), line(l) {}
};

struct Goto final : Statement {
    int markerNumber;
    int markerLine;
    int line;

    Goto(const int mn, const int ml, const int l)
        : markerNumber(mn), markerLine(ml), line(l) {}
};

struct If final : Statement {
    int variable;
    int constant;
    int markerNumber;
    int markerLine;
    int line;

    If(const int v, const int c, const int mn, const int ml, const int l)
        : variable(v), constant(c), markerNumber(mn), markerLine(ml), line(l) {}
};

struct Halt final : Statement {
    int markerLine;
    int line;

    Halt(const int ml, const int l)
        : markerLine(ml), line(l) {}
};


#endif //LWGPP_STATEMENT_H