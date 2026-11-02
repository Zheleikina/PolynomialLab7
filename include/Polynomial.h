#pragma once
#include <cstddef>
#include <stdexcept>
#include <vector>

// Інваріанти:
//  1) coeffs_ не порожній;
//  2) усі коефіцієнти скінченні;
//  3) розмір <= kMaxCapacity;
//  4) поліном нормалізований: старший коефіцієнт ненульовий (крім нульового полінома {0}).
class Polynomial {
private:
    std::vector<double> coeffs_;

    static void validateCoefficients(const std::vector<double>& coeffs);
    static void normalize(std::vector<double>& v) noexcept;

public:
    static constexpr std::size_t kMaxCapacity = 10000;

    // Гарантія: nothrow-за-змістом (стан не залежить від винятків; виділення 1 елемента)
    Polynomial();
    // Гарантія: strong (при винятку об'єкт не створюється)
    explicit Polynomial(const std::vector<double>& coeffs);
    // Гарантія: strong (при винятку об'єкт не створюється)
    Polynomial(const double* arr, int size);

    // Гарантія: no-throw
    int degree() const noexcept;
    int size() const noexcept;
    const std::vector<double>& coefficients() const noexcept;

    // Гарантія: strong (const, стан не змінюється). Виняток: std::out_of_range
    double getCoefficient(std::size_t index) const;

    // Гарантія: strong (copy-and-swap)
    void setCoefficient(std::size_t index, double value);

    // Гарантія: strong (const методи не змінюють стан об'єкта)
    double evaluate(double x) const;
    Polynomial derivative() const;
    Polynomial integral(double constant = 0.0) const;
    Polynomial add(const Polynomial& other) const;
    Polynomial multiply(const Polynomial& other) const;

    // Гарантія: strong (prepare-then-commit: результат у тимчасовому об'єкті, потім noexcept swap)
    Polynomial& operator*=(const Polynomial& other);

    // Гарантія: strong (copy-and-swap)
    void setCoefficients(const std::vector<double>& newCoeffs);
};