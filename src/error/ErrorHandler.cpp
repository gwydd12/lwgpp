#include "ErrorHandler.h"
#include <iostream>

bool ErrorHandler::hadError = false;

void ErrorHandler::report(int line, const std::string& message) {
    std::cerr << "[Zeile " << line << "] Fehler: " << message << std::endl;
    hadError = true;
}
