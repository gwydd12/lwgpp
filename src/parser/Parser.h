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

class Parser {
protected:
    std::deque<Token> tokens;
    int lastLine = 1;

    [[nodiscard]] bool isAtEnd() const;
    [[nodiscard]] const Token& peek() const;
    Token consumeToken();

    Token expectDynamic(DynamicTokenType expectedType);
    Token expectStatic(StaticTokenType expectedType);
    Token expectOneOfStatic(std::initializer_list<StaticTokenType> types);
    Token expectOneOfDynamic(std::initializer_list<DynamicTokenType> types);

    void skipToNextLine();
    void validateSemicolon();
    void setTokens(std::vector<Token> t);

    std::unique_ptr<Assignment> parseAssignment(int line);

public:
    virtual ~Parser() = default;
    virtual std::vector<std::unique_ptr<Statement>>
    parse(std::vector<Token> tokens) = 0;
};

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
    std::unordered_set<int> markerNumbers_;
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