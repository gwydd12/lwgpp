#include <algorithm>

#include "../ast/Statement.cpp"
#include "../token/Token.cpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>


class Parser {
protected:
    std::vector<Token> tokens;
    size_t current = 0;
    int lastLine = 1;

    Token peek() {
        if (isAtEnd()) {
            throw std::runtime_error("Unexpected end of input.");
        }
        return tokens[current];
    }

    Token& expectDynamic(DynamicTokenType expectedType) {
        if (!std::holds_alternative<DynamicToken>(tokens[current].value)) {
            throw std::runtime_error("Expected dynamic token");
        }
        if (std::get<DynamicToken>(tokens[current].value).type != expectedType) {
            throw std::runtime_error("Expected " + std::to_string(static_cast<int>(expectedType)));
        }
        return tokens[current++];
    }

    Token& expectStatic(StaticTokenType expectedType) {
        if (!std::holds_alternative<StaticToken>(tokens[current].value)) {
            throw std::runtime_error("Expected static token");
        }
        if (std::get<StaticToken>(tokens[current].value).type != expectedType) {
            throw std::runtime_error("Unexpected static token type");

        }
        return tokens[current++];
    }

    Token& expectOneOfStatic(std::initializer_list<StaticTokenType> types) {
        if (!std::holds_alternative<StaticToken>(tokens[current].value)) {
            throw std::runtime_error("Expected static token");
        }
        if (const auto st = std::get<StaticToken>(tokens[current].value).type; std::find(types.begin(), types.end(), st) == types.end()) {
            throw std::runtime_error("Unexpected operator");
        }
        return tokens[current++];
    }

    Token& expectOneOfDynamic(std::initializer_list<DynamicTokenType> types) {
        if (!std::holds_alternative<DynamicToken>(tokens[current].value)) {
            throw std::runtime_error("Expected dynamic token");
        }
        if (const auto dt = std::get<DynamicToken>(tokens[current].value).type; std::find(types.begin(), types.end(), dt) == types.end()) {
            throw std::runtime_error("Unexpected dynamic token type");
        }
        return tokens[current++];
    }



    [[nodiscard]] bool isAtEnd() const {
        return tokens.empty();
    }
    void skipToNextLine();
    void validateSemicolon();
    void setTokens(std::vector<Token> t) {
        tokens = std::move(t);
        current = 0;
    }

    std::unique_ptr<Assignment> parseAssignment(int line) {
        Token assigneeVariable = expectDynamic(DynamicTokenType::VARIABLE);
        expectStatic(StaticTokenType::EQUALS);
        Token assignVariable = expectDynamic(DynamicTokenType::VARIABLE);
        const Token op = expectOneOfStatic({StaticTokenType::PLUS, StaticTokenType::MINUS});
        Token constant = expectDynamic(DynamicTokenType::CONSTANT);

        return std::make_unique<Assignment>(
            assigneeVariable.getStringValue(),
            assignVariable.getStringValue(),
            getOperator(op),
            constant.getIntValue(),
            line);
    }

public:
    virtual ~Parser() = default;

    // Pure virtual - subclasses implement their specific parsing logic
    virtual std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) = 0;
};



class LWParser final : public Parser {
public:
    std::vector<std::unique_ptr<Statement>> parse(std::vector<Token> tokens) override {
        setTokens(std::move(tokens));
        std::vector<std::unique_ptr<Statement>> statements;

        while (!isAtEnd()) {
            Token currentToken = peek();
            lastLine = currentToken.line;

            if (currentToken.isStatic()) {
                StaticTokenType type = currentToken.getStatic().type;
                switch (type) {
                    case StaticTokenType::LOOP:
                        statements.push_back(parseLoop());
                        break;
                    case StaticTokenType::WHILE:
                        statements.push_back(parseWhile());
                        break;
                    case StaticTokenType::END:
                        parseEnd();
                        break;
                    case StaticTokenType::SEMICOLON:
                        expectStatic(StaticTokenType::SEMICOLON);
                        break;
                    default:
                        throw std::runtime_error("Unexpected static token at line " + std::to_string(currentToken.line));
                }
            } else if (currentToken.isDynamic()) {
                statements.push_back(parseAssignment(currentToken.line));
            } else {
                throw std::runtime_error("Unexpected token at line " + std::to_string(currentToken.line));
            }
        }

        if (!balancedIteration.empty() && !encounteredEnd) {
            throw std::runtime_error("Missing END statement for opened LOOP/WHILE");
        }

        return statements;
    }

private:
    std::deque<StaticTokenType> balancedIteration;
    bool encounteredEnd = false;

    std::vector<std::unique_ptr<Statement>> parseLW(std::vector<Token> &tokens)  {

    }

    std::unique_ptr<Loop> parseLoop() {
        balancedIteration.push_back(StaticTokenType::LOOP);
        expectStatic(StaticTokenType::LOOP);
        Token conditionToken = expectOneOfDynamic({DynamicTokenType::VARIABLE, DynamicTokenType::CONSTANT});
        expectStatic(StaticTokenType::DO);

        return std::make_unique<Loop>(
            conditionToken.getDynamic().type == DynamicTokenType::CONSTANT,
            conditionToken.getDynamic().value,
            parseLW(tokens), // TODO: find better way to pass body statements.
            conditionToken.line
            );
    }

    std::unique_ptr<While> parseWhile() {
        balancedIteration.push_back(StaticTokenType::WHILE);
        expectStatic(StaticTokenType::WHILE);
        Token variableToken = expectDynamic(DynamicTokenType::VARIABLE);
        expectStatic(StaticTokenType::GREATER_THAN);
        Token constantToken = expectDynamic(DynamicTokenType::CONSTANT);
        expectStatic(StaticTokenType::DO);

        return std::make_unique<While>(
            variableToken.getDynamic().value,
            std::stoi(constantToken.getDynamic().value),
            parseLW(tokens), // TODO: find better way to pass body statements.
            variableToken.line
            );
    }
    void parseEnd() {
        if (!isBalancedStatementSequence({StaticTokenType::LOOP, StaticTokenType::WHILE})) {
            throw std::runtime_error("Unmatched END statement");
        }
        expectStatic(StaticTokenType::END);
        encounteredEnd = true;
    }
    void validateClosingSequence(int line) {
        if (!isBalancedStatementSequence({StaticTokenType::LOOP, StaticTokenType::WHILE})) {
            throw std::runtime_error("Unmatched END statement at line " + std::to_string(line));
        }
    }

    bool isBalancedStatementSequence(std::initializer_list<StaticTokenType> expectedTypes) {
        bool found = false;
        for (const auto type : expectedTypes) {
            if (!balancedIteration.empty() && balancedIteration.back() == type) {
                balancedIteration.pop_back();
                found = true;
                break;
            }
        }
        return found;
    }
};


class GOTOParser final : public Parser {
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