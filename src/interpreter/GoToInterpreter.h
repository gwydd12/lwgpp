
#include "Interpreter.h"
#include <vector>
#include <map>

class Statement;
class Assignment;
class If;
class Goto;
class Halt;

class GoToInterpreter : public Interpreter {
private:
    int programCounter;
    std::vector<Statement*> statementList;
    std::map<int, int> markerLineMap;

    /**
     * Executes the statements <br>
     * <p>
     * This method executes the statements in the statement list. It iterates through the list of statements
     * and interprets each statement until the program counter is out of bounds or a Halt statement is reached.
     */
    void executeStatements();

    /**
     * Interprets a statement <br>
     * <p>
     * A statement is a sequence of code that can be executed. The method uses switch statement to determine
     * the type of the statement and calls the appropriate method to interpret it.
     *
     * @param statement the current statement to interpret
     */
    void interpretStatement(const Statement& statement);

    /**
     * Interprets an If statement <br>
     * <p>
     * This method interprets an If statement. The variable is the variable to compare with the constant.
     * If the variable equals the constant, the program counter is set to the marker line.
     *
     * @param ifStatement the If statement to interpret
     */
    void interpretIf(const If& ifStatement);

    /**
     * Interprets a Goto statement <br>
     * Sets the program counter to the marker line of the Goto statement.
     *
     * @param gotoStatement the Goto statement to interpret
     */
    void interpretGoto(const Goto& gotoStatement);

    /**
     * Finds the line index for a given marker number
     */
    int findLineWithMarker(int marker);

public:
    /**
     * Creates a new GoToInterpreter <br>
     * <p>
     * Initializes the environment and sets the statement list to an empty list.
     *
     * @param environment the environment to use
     */
    explicit GoToInterpreter(Environment& env);

    /**
     * Sets the marker line map
     *
     * @param markerLineMap mapping of marker numbers to line numbers
     */
    void setMarkerLineMap(const std::map<int, int>& markerLineMap);

    /**
     * Interprets a list of statements <br>
     * <p>
     * It checks if the list of statements is not null or empty
     * and then calls the executeStatements method to interpret the statements.
     *
     * @param statements the list of statements to interpret
     */
    void interpret(const std::vector<Statement*>& statements) override;

    /**
     * Interprets a list of statements asynchronously <br>
     * <p>
     * The method interprets a list of statements asynchronously by running the interpretation in a separate thread.
     *
     * @param statements the list of statements to interpret
     * @return a future representing the asynchronous interpretation
     */
    std::future<void> interpretAsync(const std::vector<Statement*>& statements) override;
};
