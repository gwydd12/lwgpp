#include <iostream>
#include <memory>
#include <vector>

#include "interpreter/GotoInterpreter.h"
#include "interpreter/LWInterpreter.h"
#include "lexer/GotoScanner.h"
#include "lexer/LwScanner.h"
#include "parser/GotoParser.h"
#include "token/Token.h"
#include "parser/LwParser.h"

void testGotoInterpreter() {
    std::string gotoSourceCode = R"(M1: x1 = x1 + 4;
    M2: x2 = x2 + 6;

    M3: x0 = x1 + 0;
    M4: If x2 = 0 Then Goto M8;
    M5: x2 = x2 - 1;
    M6: x0 = x0 + 1;
    M7: Goto M4;
    M8: Halt
    )";

    const auto lexer = std::make_unique<GotoScanner>(
    gotoSourceCode
    );
    std::vector<Token> const tokens = lexer->scanProgram();

    // Use move semantics to transfer ownership of the parser
    // goto_parser::GotoParser is a prvalue (pure rvalue) now it implicitly calls the move constructor
    const auto parser = std::make_unique<goto_parser::GotoParser>(goto_parser::GotoParser());

    const auto stmts = parser->parse(tokens);
    std::cout << "Parsed " << stmts.size() << " statements." << std::endl;

    const auto interpreter = std::make_unique<lwgpp::interp::GotoInterpreter>(Environment{});
    interpreter->setMarkerLineMap(parser->getMarkerLineMap());
    interpreter->interpret(stmts);

    std::map<std::string, int> variables = interpreter->environment().getVariables();
    std::cout << "Variables:" << std::endl;
    for (const auto& [var, value] : variables) {
        std::cout << var << " = " << value << std::endl;
    }

    if (const auto statsOpt = interpreter->environment().getMemoryStats(); statsOpt.has_value()) {
        std::cout << "Memory Stats:" << std::endl;
        std::cout << statsOpt.value() << std::endl;
    }
}

void testLWInterpreter() {
    std::string LWSourceCode = R"(
    x0 = x0 + 3;
    x1 = x1 + 2;

    x1 = x1 - 1;
    x9 = x0 + 0;
    Loop x1 Do
      Loop x9 Do
        x0 = x0 + 1;
      End;
    End
        )";

    const auto lexer = std::make_unique<LWScanner>(LWSourceCode);
    std::vector<Token> const tokens = lexer->scanProgram();

    const auto parser = std::make_unique<lw_parser::LwParser>();
    const auto stmts = parser->parse(tokens);
    std::cout << "Parsed " << stmts.size() << " statements." << std::endl;

    Environment env{}; // initialise empty environment
    auto interpreter = std::make_unique<lwgpp::interp::LWInterpreter>(std::move(env));
    //interpreter->setMarkerLineMap(parser->getMarkerLineMap());
    interpreter->interpret(stmts);

    auto variables = interpreter->environment().getVariables();
    std::cout << "Variables:" << std::endl;
    for (const auto& [var, value] : variables) {
        std::cout << var << " = " << value << std::endl;
    }

    if (const auto stats = interpreter->environment().getMemoryStats(); stats.has_value()) {
        std::cout << "Memory Stats:" << std::endl;
        std::cout << stats.value() << std::endl;
    }
}

int main() {
    std::cout << "Testing Goto Interpreter:" << std::endl;
    testGotoInterpreter();
    std::cout << "\nTesting LW Interpreter:" << std::endl;
    testLWInterpreter();
    return 0;
}