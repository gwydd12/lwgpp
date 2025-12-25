#include "Debugger.h"

// Static member definitions
std::vector<Debugger::StepMap> Debugger::debugVariables;
int Debugger::currentStep = -1;
bool Debugger::firstStep = true;
bool Debugger::lastStep = false;

void Debugger::addDebugVariable(int line, const VarsMap& variables) {
    StepMap step;
    step.emplace(line, variables);
    debugVariables.push_back(std::move(step));
}

void Debugger::reset() {
    debugVariables.clear();
    currentStep = -1;
    firstStep = true;
    lastStep = false;
}

Debugger::StepMap Debugger::nextStep() {
    if (debugVariables.empty()) return StepMap{};

    if (currentStep == static_cast<int>(debugVariables.size()) - 1) {
        lastStep = true;
    } else {
        ++currentStep;
        firstStep = false;
    }
    return debugVariables[std::max(0, currentStep)];
}

Debugger::StepMap Debugger::previousStep() {
    if (debugVariables.empty()) return StepMap{};

    if (currentStep <= 0) {
        firstStep = true;
        currentStep = 0;
    } else {
        --currentStep;
        lastStep = false;
    }
    return debugVariables[currentStep];
}
