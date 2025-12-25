#include <iostream>
#include <memory>
#include <vector>

#include "lexer/LwScanner.h"
#include "parser/Parser.h"
#include "token/Token.h"

int main() {
    const auto lexer = std::make_unique<LWScanner>(
        "Loop x1 Do x1 = x1 + 1; End"
    );
    std::vector<Token> const tokens = lexer->scanProgram();
    const auto parser = std::make_unique<LWParser>();

    const auto stmts = parser->parse(tokens);
    std::cout << "Parsed " << stmts.size() << " statements." << std::endl;
    return 0;
}