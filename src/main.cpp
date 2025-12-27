#include <iostream>
#include <memory>
#include <vector>

#include "interpreter/LWInterpreter.h"
#include "lexer/LwScanner.h"
#include "parser/Parser.h"
#include "token/Token.h"

int main() {
    std::string sourceCode = R"(
    x1 = x1 + 3;
    Loop x1 Do
      x0 = x0 + 1;
    End
    )";
    const auto lexer = std::make_unique<LWScanner>(
        sourceCode
    );
    std::vector<Token> const tokens = lexer->scanProgram();
    const auto parser = std::make_unique<LWParser>();

    const auto stmts = parser->parse(tokens);
    std::cout << "Parsed " << stmts.size() << " statements." << std::endl;

    const auto interpreter = std::make_unique<LWInterpreter>(Environment{});
    interpreter->interpret(stmts);

    std::map<std::string, int> variables = LWInterpreter::environment.getVariables();
    std::cout << "Variables:" << std::endl;
    for (const auto& [var, value] : variables) {
        std::cout << var << " = " << value << std::endl;
    }
    return 0;
}