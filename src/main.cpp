#include <iostream>
#include <vector>
#include "lexer/LWScanner.h"
#include "error/ErrorHandler.h"
#include "lexer/GotoScanner.h"

void gotoTest() {
    std::cout << "=== GotoScanner Test ===" << std::endl;

    // Example Goto Code
    std::string sourceCode = R"(M23489: ;    Halt; x30942 = + - Goto Halt If Then M0 25432)";

    // Scanner initialization
    GotoScanner scanner(sourceCode);

    // Scan start
    auto tokens = scanner.scanProgram();

    if (ErrorHandler::hadError) {
        std::cerr << "Error while Scanning!" << std::endl;
    }

    // Ausgabe
    std::cout << "\nFound Goto Tokens:\n";
    for (const auto &token: tokens) {
        std::cout << token.toString() << std::endl;
    }

    std::cout << "\n=== GotoScan done ===" << std::endl;
}

void lwTest() {
    std::cout << "=== LWScanner Test ===" << std::endl;

    // Example Code
    std::string sourceCode = R"(
        Loop x1 = 5;
        While x1 > 0;
        Do
            x1 = x1 - 1;
        End
        End
    )";

    // Scanner initialization
    LWScanner scanner(sourceCode);

    // start Scan
    auto tokens = scanner.scanProgram();

    if (ErrorHandler::hadError) {
        std::cerr << "Error while Scanning!" << std::endl;
    }

    // Output found Tokens
    std::cout << "\nFound LW Tokens:\n";
    for (const auto &token: tokens) {
        std::cout << token.toString() << std::endl;
    }

    std::cout << "\n=== LW Scan done ===" << std::endl;
}

int main() {
    lwTest();
    gotoTest();
    return 0;
}
