#ifndef LWGPP_PARSER_H
#define LWGPP_PARSER_H

#include "../token/Token.h"
#include "../ast/Statement.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>

class Parser {
protected:
    std::vector<Token> tokens;
    int current = 0;
    int lastLine = 1;

    Token peek();
    Token& expectDynamic(DynamicTokenType expectedType);
    Token& expectStatic(StaticTokenType expectedType);
    Token& expectOneOfStatic(std::initializer_list<StaticTokenType> types);
    Token& expectOneOfDynamic(std::initializer_list<DynamicTokenType> types);

    [[nodiscard]] bool isAtEnd() const;
    void skipToNextLine();
    void validateSemicolon();
    void setTokens(std::vector<Token> t);

    std::unique_ptr<Assignment> parseAssignment(int line);

public:
    virtual ~Parser() = default;
    virtual std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) = 0;
};


class LWParser final : public Parser {
public:
    std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) override;

private:
    std::deque<StaticTokenType> balancedIteration;
    bool encounteredEnd = false;

    std::vector<std::unique_ptr<Statement>> parseLW(std::vector<Token> &tokens);
    std::unique_ptr<Loop> parseLoop();
    std::unique_ptr<While> parseWhile();
    void parseEnd();
    void validateClosingSequence(int line);
    bool isBalancedStatementSequence(std::initializer_list<StaticTokenType> expectedTypes);
};


class GOTOParser final : public Parser {
public:
    std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) override;
    const std::unordered_map<int, int>& getMarkerLineMap() const;

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

#endif //LWGPP_PARSER_H