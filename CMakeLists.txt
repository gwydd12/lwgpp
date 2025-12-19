cmake_minimum_required(VERSION 4.0)
project(lwgpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(lwgpp src/main.cpp
        src/token/TokenType.h
        src/token/Token.cpp
        src/error/ErrorHandler.cpp
        src/error/ErrorHandler.h
        src/lexer/Scanner.cpp
        src/lexer/Scanner.h
        src/lexer/LwScanner.cpp
        src/lexer/LwScanner.h
        src/token/Token.h
        src/lexer/GotoScanner.h
        src/lexer/GotoScanner.cpp
)
