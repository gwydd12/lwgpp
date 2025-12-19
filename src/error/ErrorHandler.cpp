#include "ErrorHandler.h"
#include <iostream>

bool ErrorHandler::hadError = false;

void ErrorHandler::report(int line, const std::string& message) {
    std::cerr << "[Line " << line << "] Error: " << message << std::endl;
    hadError = true;
}
