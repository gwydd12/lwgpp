#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

class ErrorHandler;

class Environment {

    private:
        std::unordered_map<std::string, int> variables;
        bool debugMode;

        /**
         * Initializes a variable <br>
         * <p>
         * If the variable identifier is not found, it is initialized to 0.
         *
         * @param variable the variable identifier to initialize or get
         */
        void initVariableIfAbsent(const std::string& variable);


    public:

        /**
         * Creates a new environment with an empty variables map
         * and debug mode disabled.
         */
        Environment();

        /**
         * Creates a new environment with an empty variables map
         * and the specified debug mode.
         *
         * @param debugMode the debug mode to use
         */
        explicit Environment(bool debugMode);

        /**
         * Creates a new environment with the specified variables map
         * and the specified debug mode.
         *
         * @param variables the variables map to use
         * @param debugMode the debug mode to use
         */
        Environment(const std::unordered_map<std::string, int>& variables, bool debugMode);

        /**
         * A helper method to initialize variables
         *
         * @param variables the variables to initialize
         */
        void initVariablesIfAbsent(const std::initializer_list<std::string>& vars);

        /**
         * Sets a variable to a specific value <br>
         * <p>
         * If the value is negative, an error is reported.
         *
         * @param line the line number for error reporting
         * @param variable the variable identifier
         * @param value    the value to set the variable to
         */
        void setVariable(int line, const std::string& variable, int value);

        /**
         * Sets a variable to a specific value <br>
         * <p>
         * If the value is negative, an error is reported.
         *
         * @param variable the variable identifier
         * @param value    the value to set the variable to
         */
        void setVariable(const std::string& variable, int value);

        /**
         * Gets the value of a variable <br>
         * <p>
         * If the variable is not found, it is initialized to 0.
         *
         * @param variable the variable identifier
         * @return the value of the variable
         */
        int getVariable(const std::string& variable);

        /**
         * Gets the variables map
         *
         * @return the variables map
         */
        std::unordered_map<std::string, int> getVariables() const;

        /**
         * Sets variables from a map
         */
        void setVariables(const std::unordered_map<std::string, int>& vars);

        /**
         * Adds debug variables in debug mode
         */
        void addDebugVariablesInDebugMode(int line);
    

};