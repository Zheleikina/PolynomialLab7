#pragma once
#include <iostream>
#include <string>

class ScopeLogger {
private:
    std::string name_;
public:
    explicit ScopeLogger(std::string name) : name_(std::move(name)) {
        std::cout << "[ScopeLogger] Entering scope: " << name_ << '\n';
    }

    ~ScopeLogger() {
        std::cout << "[ScopeLogger] Leaving scope (Stack Unwinding): " << name_ << '\n';
    }

    // Забороняємо копіювання для уникнення некоректного логування
    ScopeLogger(const ScopeLogger&) = delete;
    ScopeLogger& operator=(const ScopeLogger&) = delete;
};