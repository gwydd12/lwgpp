#ifndef LWGPP_PARSER_H
#define LWGPP_PARSER_H

#include "../token/Token.h"
#include "../ast/Statement.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <initializer_list>
#include <map>

/**
 * Parser definitions for the GOTO and LW languages
 *
 * Responsibilities:
 * - Define a base Parser class with common parsing utilities
 * - Implement GOTOParser and LWParser derived classes for specific language parsing
 * - Provide methods to parse different statement types and validate syntax
 */

class Parser {
protected:

    /*
    * std::deque container is used to store tokens since it provides 
    * efficient pop_front operation(O(1)) in consumeToken() function 
    * as compared to std::vector (O(n)) 
    */
    std::deque<Token> tokens;
    int lastLine = 1; // Tracks the line number of the last processed token for GoTOParser

    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token& peek() const;
    Token consumeToken();

    // std::initializer_list used for passing a list of expected types

    Token expectDynamic(DynamicTokenType expectedType);
    Token expectStatic(StaticTokenType expectedType);
    Token expectOneOfStatic(std::initializer_list<StaticTokenType> types);
    Token expectOneOfDynamic(std::initializer_list<DynamicTokenType> types);

    void skipToNextLine();
    void validateSemicolon(); // Ensures a semicolon follows the current statement
    void setTokens(std::vector<Token> t);

    std::unique_ptr<Assignment> parseAssignment(int line); 

public:
    virtual ~Parser() = default;
    virtual std::vector<std::unique_ptr<Statement>>
    parse(std::vector<Token> tokens) = 0;
};

/*
    * Use of unique ptr (smart pointer) for the different statement types
    * Ensures proper memory management and avoids memory leaks
    * 
    * std::vector is used to hold smart pointers to statements for
    * dynamic sizing
    * 
    * Inheritance for code reuse and specialization
    * Polymorphism for dynamic method resolution
*/

class LWParser final : public Parser {
public:
    std::vector<std::unique_ptr<Statement>>
    parse(std::vector<Token> tokens) override;

private:
    std::deque<StaticTokenType> balancedIteration;
    bool encounteredEnd = false;

    std::vector<std::unique_ptr<Statement>> parseLW();
    std::unique_ptr<Loop> parseLoop();
    std::unique_ptr<While> parseWhile();
    void parseEnd();
    void validateClosingSequence(int line);

    bool isBalancedStatementSequence(
        std::initializer_list<StaticTokenType> expectedTypes
    );
};

class GOTOParser final : public Parser {
public:
    std::vector<std::unique_ptr<Statement>>
    parse(std::vector<Token> tokens) override;

    std::map<std::string, int> getMarkerLineMap();

private:
    std::unordered_set<int> markerNumbers_;  // Concept: unordered_set to track unique marker numbers
    std::map<std::string, int> gotoValuesMap_;
    std::map<std::string, int> markerLineMap_;
    bool containsHalt_ = false;

    std::vector<std::unique_ptr<Statement>> parseGoto();
    std::unique_ptr<Halt> parseHalt(int line, int markerLine);
    std::unique_ptr<If> parseIf(int line, int markerLine);
    std::unique_ptr<Goto> parseGotoStatement(int line, int markerLine);
    void checkGotoValues();
};

#endif // LWGPP_PARSER_H