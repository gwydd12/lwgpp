cmake_minimum_required(VERSION 4.0)
project(lwgpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(lwgpp src/main.cpp
        src/token/Token.cpp
        src/parser/Parser.cpp
        src/ast/Statement.cpp
)
