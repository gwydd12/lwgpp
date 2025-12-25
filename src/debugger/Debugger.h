#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <string>

class Debugger {
public:
    // Each step is a map: line -> variables map (variable name -> value)
    using VarsMap = std::unordered_map<std::string,int>;
    using StepMap = std::map<int, VarsMap>;

    static std::vector<StepMap> debugVariables;
    static int currentStep;
    static bool firstStep;
    static bool lastStep;

    Debugger() = delete;

    static void addDebugVariable(int line, const VarsMap& variables);
    static void reset();
    static StepMap nextStep();
    static StepMap previousStep();
};
