#include <iostream>
#include <memory>
#include <vector>

#include "interpreter/GotoInterpreter.h"
#include "interpreter/LWInterpreter.h"
#include "lexer/GotoScanner.h"
#include "lexer/LwScanner.h"
#include "parser/GotoParser.h"
#include "parser/Parser.h"
#include "token/Token.h"

int main() {
    std::string GotoSourceCode = R"(M1: x1 = x1 + 4;
M2: x2 = x2 + 6;

M3: x0 = x1 + 0;
M4: If x2 = 0 Then Goto M8;
M5: x2 = x2 - 1;
M6: x0 = x0 + 1;
M7: Goto M4;
M8: Halt
    )";
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
    const auto lexer = std::make_unique<GotoScanner>(
        GotoSourceCode
    );
    std::vector<Token> const tokens = lexer->scanProgram();

    // Use move semantics to transfer ownership of the parser
    // goto_parser::GotoParser is a prvalue (pure rvalue) now it implicitly calls the move constructor
    const auto parser = std::make_unique<goto_parser::GotoParser>(goto_parser::GotoParser());

    const auto stmts = parser->parse(tokens);
    std::cout << "Parsed " << stmts.size() << " statements." << std::endl;

    const auto interpreter = std::make_unique<GotoInterpreter>(Environment{});
    interpreter->setMarkerLineMap(parser->getMarkerLineMap());
    interpreter->interpret(stmts);

    std::map<std::string, int> variables = GotoInterpreter::environment.getVariables();
    std::cout << "Variables:" << std::endl;
    for (const auto& [var, value] : variables) {
        std::cout << var << " = " << value << std::endl;
    }
    return 0;
}