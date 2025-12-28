//
// Created by philipp.hennken on 30.10.2025.
//

#ifndef LWGPP_ERRORHANDLER_H
#define LWGPP_ERRORHANDLER_H
#include <iostream>


class ErrorHandler {
public:
    static bool hadError;

    static void report(int line, const std::string &message);
};


#endif
