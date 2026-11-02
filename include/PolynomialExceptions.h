#pragma once
#include <stdexcept>
#include <string>

// Базовий клас винятків бібліотеки (вимога: успадкований від std::runtime_error)
class PolynomialException : public std::runtime_error {
public:
    explicit PolynomialException(const std::string& message)
        : std::runtime_error(message) {}
};

// Спеціалізований клас для недійсних коефіцієнтів
class InvalidCoefficientException : public PolynomialException {
private:
    std::string parameter_;
    double value_;
public:
    InvalidCoefficientException(const std::string& parameter, double value)
        : PolynomialException("Invalid polynomial coefficient for: " + parameter),
        parameter_(parameter), value_(value) {}

    const std::string& parameter() const noexcept { return parameter_; }
    double value() const noexcept { return value_; }
};

// Спеціалізований клас для перевищення місткості або розміру
class PolynomialCapacityException : public PolynomialException {
private:
    int capacity_;
    int requested_;
public:
    PolynomialCapacityException(int capacity, int requested)
        : PolynomialException("Polynomial capacity limit exceeded"),
        capacity_(capacity), requested_(requested) {}

    int capacity() const noexcept { return capacity_; }
    int requested() const noexcept { return requested_; }
};