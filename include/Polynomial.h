#pragma once
#include <vector>
#include <cstddef>
#include <stdexcept>

class Polynomial {
private:
    std::vector<double> coeffs_;
    void validateCoefficients(const std::vector<double>& coeffs) const;

public:
    static constexpr size_t kMaxCapacity = 10000;

    Polynomial();
    explicit Polynomial(const std::vector<double>& coeffs);
    Polynomial(const double* arr, int size);

    int degree() const noexcept;
    int size() const noexcept;
    const std::vector<double>& coefficients() const noexcept;

    // Доступ за індексом (генерує std::out_of_range у разі невалідного індексу)
    double getCoefficient(size_t index) const;
    void setCoefficient(size_t index, double value);

    double evaluate(double x) const;
    Polynomial derivative() const;
    Polynomial integral(double constant = 0.0) const;

    Polynomial add(const Polynomial& other) const;
    Polynomial multiply(const Polynomial& other) const;

    // Сильна гарантія: множення з присвоєнням (operator*=)
    Polynomial& operator*=(const Polynomial& other);

    void setCoefficients(const std::vector<double>& newCoeffs);
};