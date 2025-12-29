#ifndef LWGPP_GOTOINTERPRETER_H
#define LWGPP_GOTOINTERPRETER_H
#include "Interpreter.h"

/**
 * Concrete interpreter implementing a control-flow model
 * based on GOTO and IF statements.
 *
 * This interpreter executes statements sequentially using
 * an explicit program counter and supports non-linear
 * control flow via markers.
 */
class GotoInterpreter: public Interpreter {
public:
    /**
     * Owns the list of statements representing the program.
     *
     * Advanced concepts:
     * - RAII
     * - Smart Pointer idiom (std::unique_ptr)
     */
    std::vector<std::unique_ptr<Statement>> statements;

    /**
     * Constructs a GotoInterpreter with a given execution environment.
     * Initializes the program counter to zero.
     *
     * @param env Initial execution environment
     */
    explicit GotoInterpreter(const Environment& env)
        : Interpreter(env) {
        pc_ = 0;
    }

    /**
     * Entry point for interpreting a program.
     * Overrides the abstract interpret method of Interpreter.
     *
     * @param statements Sequence of AST statements
     */
    void interpret(const std::vector<std::unique_ptr<Statement>>& statements) override;

    /**
     * Executes a list of statements using the internal program counter.
     *
     * @param stmts Statements to execute
     */
    void executeStatements(const std::vector<std::unique_ptr<Statement>> &stmts);

    /**
     * Dispatches execution based on the concrete statement type.
     *
     * @param statement AST statement to interpret
     */
    void interpretStatement(const Statement &statement);

    /**
     * Executes an IF statement and conditionally modifies
     * the program counter.
     *
     * @param ifStmt IF-statement AST node
     */
    void interpretIf(const If& ifStmt);

    /**
     * Executes a GOTO statement by updating the program counter.
     *
     * @param gotoStmt GOTO-statement AST node
     */
    void interpretGoto(const Goto& gotoStmt);

    /**
     * Sets the mapping between marker names and source-code line numbers.
     *
     * @param markerLineMap Map of marker identifiers to line numbers
     */
    void setMarkerLineMap(const std::map<std::string, int>& markerLineMap) {
        markerLineMap_ = markerLineMap;
    }

    /**
     * Finds the line number associated with a marker.
     *
     * @param marker Marker identifier
     * @return Line number of the marker
     */
    [[nodiscard]] int findLineWithMarker(const std::string &marker) const;

private:
    /**
     * Program counter indicating the index of the next statement to execute.
     */
    int pc_;

    /**
     * Indicates whether program execution has been halted.
     *
     * Advanced concepts:
     * - Execution state management
     */
    bool isHalted_;

    /**
     * Maps marker identifiers to their corresponding statement indices.
     *
     * Advanced concepts:
     * - std::map (associative container)
     * - Control-flow resolution
     */
    std::map<std::string, int> markerLineMap_;
};

#endif //LWGPP_GOTOINTERPRETER_H