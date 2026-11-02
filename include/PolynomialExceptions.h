#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>

// Базовий клас винятків бібліотеки (успадкований від std::runtime_error)
class PolynomialException : public std::runtime_error {
public:
    explicit PolynomialException(const std::string& message)
        : std::runtime_error(message) {}
};

// Недійсний (NaN/Inf) коефіцієнт
class InvalidCoefficientException : public PolynomialException {
private:
    std::string parameter_;
    double value_;
public:
    InvalidCoefficientException(const std::string& parameter, double value)
        : PolynomialException("Invalid polynomial coefficient " + parameter +
            " (value = " + std::to_string(value) + ")"),
        parameter_(parameter), value_(value) {}

    const std::string& parameter() const noexcept { return parameter_; }
    double value() const noexcept { return value_; }
};

// Перевищення максимальної місткості полінома
class PolynomialCapacityException : public PolynomialException {
private:
    std::size_t capacity_;
    std::size_t requested_;
public:
    PolynomialCapacityException(std::size_t capacity, std::size_t requested)
        : PolynomialException("Polynomial capacity limit exceeded: capacity = " +
            std::to_string(capacity) + ", requested = " +
            std::to_string(requested)),
        capacity_(capacity), requested_(requested) {}

    std::size_t capacity() const noexcept { return capacity_; }
    std::size_t requested() const noexcept { return requested_; }
};