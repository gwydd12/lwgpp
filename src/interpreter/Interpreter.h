#pragma once

#include <vector>
#include <future>
#include <atomic>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <limits>
#include <string>

class Environment;
class Statement;
class Assignment;
enum class Operator;

class Interpreter {
protected:
    Environment& environment;
    std::atomic<bool> isHalted{false};

private:
    // scheduling support
    std::mutex schedulerMutex;
    std::condition_variable schedulerCv;
    int haltTimeout{15}; // seconds

public:
    explicit Interpreter(Environment& env); // do not allow implicit conversion
    virtual ~Interpreter() = default;

    virtual void interpret(const std::vector<Statement*>& statements) = 0;
    virtual std::future<void> interpretAsync(const std::vector<Statement*>& statements);

    void halt();
    void setHaltTimeout(int seconds);
    bool is_halted() const;

protected:
    void scheduleHalt();
    void interpretAssignment(const Assignment& assignment);
};
