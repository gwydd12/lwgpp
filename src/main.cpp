#include <iostream>
#include <vector>
#include "lexer/LWScanner.h"
#include "error/ErrorHandler.h"

int main() {
    std::cout << "=== LWScanner Test ===" << std::endl;

    // Beispiel-Programmcode in deiner Loop/While-Sprache
    std::string sourceCode = R"(
        Loop x1 = 5;
        While x1 > 0;
        Do
            x1 = x1 - 1;
        End
        End
    )";

    // Scanner initialisieren
    LWScanner scanner(sourceCode);

    // Scanvorgang starten
    auto tokens = scanner.scanProgram();

    // Falls Fehler aufgetreten sind, abbrechen
    if (ErrorHandler::hadError) {
        std::cerr << "Fehler beim Scannen aufgetreten!" << std::endl;
        return 1;
    }

    // Ausgabe der erkannten Tokens
    std::cout << "\nGefundene Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }

    std::cout << "\n=== Scan abgeschlossen ===" << std::endl;
    return 0;
}
