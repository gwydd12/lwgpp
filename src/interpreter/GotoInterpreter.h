#ifndef LWGPP_GOTOINTERPRETER_H
#define LWGPP_GOTOINTERPRETER_H
#include "Interpreter.h"


class GotoInterpreter: public Interpreter {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    explicit GotoInterpreter(const Environment& env) : Interpreter(env) {pc_ = 0;}

    void interpret(const std::vector<std::unique_ptr<Statement>>& statements) override;
    void executeStatements(const std::vector<std::unique_ptr<Statement>> &stmts);
    void interpretStatement(const Statement &statement);
    void interpretIf(const If& ifStmt);
    void interpretGoto(const Goto& gotoStmt);
    void setMarkerLineMap(const  std::map<std::string, int>& markerLineMap) {
        markerLineMap_ = markerLineMap;
    }
    [[nodiscard]] int findLineWithMarker(const std::string &marker) const;




private:
    int pc_;
    bool isHalted_;
    std::map<std::string, int> markerLineMap_;
};


#endif //LWGPP_GOTOINTERPRETER_H