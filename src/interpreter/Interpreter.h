#ifndef LWGPP_INTERPRETER_H
#define LWGPP_INTERPRETER_H
#include "Environment.h"
#include "../ast/Statement.h"

/**
 * Abstract base class for interpreters executing an AST.
 * Defines the common interface and shared execution environment.
 */
class Interpreter {
public:
    /**
     * Constructs an interpreter with a given execution environment.
     * The environment is copied into a shared static instance.
     *
     * @param env Initial execution environment
     *
     * Advanced concepts:
     * - Explicit constructors (prevents implicit conversions)
     * - Const correctness
     * - Copy semantics
     */
    explicit Interpreter(const Environment& env) {
        environment = env;
    }

    /**
     * Virtual destructor to ensure proper cleanup through base-class pointers.
     */
    virtual ~Interpreter() = default;

    /**
     * Interprets a sequence of AST statements.
     * Must be implemented by concrete interpreter subclasses.
     *
     * @param statements List of statements forming a program
     */
    virtual void interpret(const std::vector<std::unique_ptr<Statement>> &statements) = 0;

    /**
     * Executes a single assignment statement.
     * Uses the shared interpreter environment.
     *
     * @param assignment Assignment AST node
     */
    static void interpretAssignment(const Assignment& assignment);

    /**
     * Shared execution environment used by all interpreter instances.
     */
    static Environment environment;
};

#endif //LWGPP_INTERPRETER_H