cmake_minimum_required(VERSION 4.0)
project(lwgpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(lwgpp src/main.cpp
        src/token/Token.cpp
        src/parser/Parser.cpp
        src/ast/Statement.cpp
        src/error/ErrorHandler.cpp
        src/error/ErrorHandler.h
        src/lexer/Scanner.cpp
        src/lexer/Scanner.h
        src/lexer/LwScanner.cpp
        src/lexer/LwScanner.h
        src/lexer/GotoScanner.h
        src/lexer/GotoScanner.cpp
        src/parser/Parser.h
        src/token/Token.h
        src/ast/Statement.h
        src/lexer/Scanner.h
        src/interpreter/Interpreter.h
        src/interpreter/Interpreter.cpp
        src/interpreter/Environment.cpp
        src/interpreter/Environment.h
        src/interpreter/LWInterpreter.cpp
        src/interpreter/LWInterpreter.h
        src/interpreter/GotoInterpreter.cpp
        src/interpreter/GotoInterpreter.h
        src/parser/LWParser.cpp
        src/parser/LWParser.h
        src/parser/GotoParser.cpp
        src/parser/GotoParser.h
)
