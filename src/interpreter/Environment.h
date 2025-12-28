#ifndef LWGPP_ENVIRONMENT_H
#define LWGPP_ENVIRONMENT_H
#include <map>
#include <string>

class Environment {
    std::map<std::string, int> variables_;
    void initVariableIfAbsent(const std::string& var) {
        if (!variables_.count(var)) {
            setVariable(var, 0);
        }
    }

public:
    explicit Environment(std::map<std::string, int>& variables) : variables_(std::move(variables)) {}
    Environment() = default;
    virtual ~Environment() = default;

    void initVariablesIfAbsent(std::initializer_list<std::string> vars);
    std::map<std::string, int> getVariables();
    void setVariable(const std::string& var, int value);
    [[nodiscard]] int getVariableValue(const std::string& var) const;

};


#endif //LWGPP_ENVIRONMENT_H