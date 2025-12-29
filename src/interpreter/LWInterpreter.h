#ifndef LWGPP_LWINTERPRETER_H
#define LWGPP_LWINTERPRETER_H
#include "Interpreter.h"

/**
 * Concrete interpreter implementing structured control flow
 * using LOOP and WHILE statements.
 *
 * This interpreter executes programs without explicit GOTO-based
 * jumps and relies on structured iteration constructs.
 */
class LWInterpreter: public Interpreter {
public:
    /**
     * Constructs the interpreter with a given execution environment.
     *
     * @param env Initial execution environment
     */
    explicit LWInterpreter(const Environment& env)
        : Interpreter(env) {}

    /**
     * Virtual destructor overriding the base-class destructor.
     */
    ~LWInterpreter() override = default;

    /**
     * Interprets a sequence of statements representing a program.
     * Overrides the abstract interpret method from Interpreter.
     *
     * @param statements Program statements
     */
    void interpret(const std::vector<std::unique_ptr<Statement>> &statements) override;

    /**
     * Dispatches interpretation based on the concrete statement type.
     *
     * @param statement AST statement to interpret
     */
    void interpretStatement(const Statement& statement);

    /**
     * Interprets a LOOP statement.
     *
     * @param loop LOOP-statement AST node
     */
    void interpretLoop(const Loop& loop);

    /**
     * Interprets a WHILE statement.
     *
     * @param whileStmt WHILE-statement AST node
     */
    void interpretWhile(const While& whileStmt);
};


#endif //LWGPP_LWINTERPRETER_H