//
// Created by stdin on 11/6/25.
//

#include "../ast/Statement.h"
#include "../token/Token.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>


class Parser {
protected:
    std::vector<Token> tokens;
    size_t current = 0;
    int lastLine = 1;

    Token& peek() const;
    const Token& consume(const std::string& message,
                        std::initializer_list<StaticTokenType> staticTypes);
    const Token& consumeDynamic(const std::string& message,
                               std::initializer_list<DynamicTokenType> dynamicTypes);
    bool check(std::initializer_list<StaticTokenType> types) const;
    bool isAtEnd() const;
    void skipToNextLine();
    void validateSemicolon();

    // Common parsing logic
    std::unique_ptr<Assignment> parseAssignment(int line) {
    }

public:
    virtual ~Parser() = default;

    // Pure virtual - subclasses implement their specific parsing logic
    virtual std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) = 0;
};

#include <stack>

class LWParser : public Parser {
public:
    std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) override;

private:
    std::stack<StaticTokenType> balancedStack;
    bool encounteredEnd = false;

    std::vector<std::unique_ptr<Statement>> parseLW();
    std::unique_ptr<Loop> parseLoop();
    std::unique_ptr<While> parseWhile();
    void parseEnd();
    void validateClosingSequence(int line);
    bool isBalancedStatementSequence(std::initializer_list<StaticTokenType> expectedTypes);
};


class GOTOParser : public Parser {
public:
    std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) override;
    const std::unordered_map<int, int>& getMarkerLineMap() const { return markerLineMap; }

private:
    std::unordered_set<int> markerNumbers;
    std::unordered_map<int, int> gotoValuesMap;
    std::unordered_map<int, int> markerLineMap;
    bool containsHalt = false;

    std::unique_ptr<Halt> parseHalt(int line, int markerLine);
    std::unique_ptr<If> parseIf(int line, int markerLine);
    std::unique_ptr<Goto> parseGotoStatement(int line, int markerLine);
    void checkGotoValues();
};