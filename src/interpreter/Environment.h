#ifndef LWGPP_ENVIRONMENT_H
#define LWGPP_ENVIRONMENT_H
#include <map>
#include <string>

/**
 * Represents the execution environment of the interpreter.
 * Stores and manages variables and their associated values.
 *
 * Advanced concepts:
 * - Encapsulation
 * - RAII
 * - Standard associative containers
 */
class Environment {

    /**
     * Mapping of variable names to their integer values.
     *
     * Advanced concepts:
     * - std::map (ordered associative container)
     */
    std::map<std::string, int> variables_;

    /**
     * Initializes a variable with a default value if it does not yet exist.
     * This function is private to enforce controlled access.
     *
     * @param var Variable name
     */
    void initVariableIfAbsent(const std::string& var) {
        if (!variables_.count(var)) {
            setVariable(var, 0);
        }
    }

public:
    /**
     * Constructs an environment from an existing variable map.
     * Takes ownership of the map using move semantics.
     *
     * @param variables Initial variable storage
     */
    explicit Environment(std::map<std::string, int>& variables)
        : variables_(std::move(variables)) {}

    /**
     * Default constructor creating an empty environment.
     */
    Environment() = default;

    /**
     * Virtual destructor to allow safe polymorphic deletion.
     */
    virtual ~Environment() = default;

    /**
     * Initializes multiple variables if they are not already present.
     *
     * @param vars List of variable names
     *
     * Advanced concepts:
     * - std::initializer_list
     */
    void initVariablesIfAbsent(std::initializer_list<std::string> vars);

    /**
     * Returns a copy of the current variable map.
     *
     * @return Map of variable names to values
     *
     * Advanced concepts:
     * - Value semantics
     * - Encapsulation (returns copy instead of reference)
     */
    std::map<std::string, int> getVariables();

    /**
     * Sets the value of a variable.
     * Initializes the variable if it does not already exist.
     *
     * @param var Variable name
     * @param value New value
     */
    void setVariable(const std::string& var, int value);

    /**
     * Retrieves the value of a variable.
     *
     * @param var Variable name
     * @return Variable value
     */
    [[nodiscard]] int getVariableValue(const std::string& var) const;

};

#endif // LWGPP_ENVIRONMENT_H
