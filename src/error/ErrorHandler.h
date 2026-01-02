//
// Created by philipp.hennken on 30.10.2025.
//

#ifndef LWGPP_ERRORHANDLER_H
#define LWGPP_ERRORHANDLER_H
#include <iostream>


class ErrorHandler {
public:
    static bool hadError;

    /**
     * Reports an error with line number and specific message.
     * Sets the hadError flag to true upon reporting an error. Allows tracking of multiple errors.
     *
     * - Static member function to maintain error state across instances.
     * - Static function gives global accessibility for error reporting and is created upon first use and destroyed at program termination.
     * @param line the line number where the error occurred
     * @param message the error message to report
     */
    static void report(int line, const std::string &message);
};


#endif
