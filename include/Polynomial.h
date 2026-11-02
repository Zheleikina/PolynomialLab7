#pragma once
#include <vector>
#include <cstddef>

class Polynomial {
private:
    std::vector<double> coeffs_;

    void validateCoefficients(const std::vector<double>& coeffs) const;

public:
    // Конструктори
    Polynomial(); // Nothrow
    explicit Polynomial(const std::vector<double>& coeffs); // Strong guarantee
    Polynomial(const double* arr, int size); // Strong guarantee

    // Геттери (відсутність винятків)
    int degree() const noexcept;
    int size() const noexcept;
    const std::vector<double>& coefficients() const noexcept;

    // Обчислювальні методи
    double evaluate(double x) const; // Basic guarantee
    Polynomial derivative() const; // Strong guarantee
    Polynomial integral(double constant = 0.0) const; // Strong guarantee

    // Арифметичні операції
    Polynomial add(const Polynomial& other) const; // Strong guarantee
    Polynomial multiply(const Polynomial& other) const; // Strong guarantee

    // Зміна стану зі сильною гарантією (All-or-nothing)
    void setCoefficients(const std::vector<double>& newCoeffs);
};