#include "Polynomial.h"
#include "PolynomialExceptions.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

void Polynomial::validateCoefficients(const std::vector<double>& coeffs) const {
    if (coeffs.empty()) {
        throw std::invalid_argument("Polynomial coefficient list cannot be empty");
    }
    if (coeffs.size() > 10000) {
        throw PolynomialCapacityException(10000, static_cast<int>(coeffs.size()));
    }
    for (size_t i = 0; i < coeffs.size(); ++i) {
        if (!std::isfinite(coeffs[i])) {
            throw InvalidCoefficientException("coeffs[" + std::to_string(i) + "]", coeffs[i]);
        }
    }
}

Polynomial::Polynomial() : coeffs_{ 0.0 } {}

Polynomial::Polynomial(const std::vector<double>& coeffs) {
    if (coeffs.empty()) {
        throw std::invalid_argument("Polynomial coefficient list cannot be empty");
    }
    validateCoefficients(coeffs);
    coeffs_ = coeffs;
}

Polynomial::Polynomial(const double* arr, int size) {
    if (!arr) {
        throw std::invalid_argument("Pointer to coefficients cannot be nullptr");
    }
    if (size <= 0 || size > 10000) {
        throw PolynomialCapacityException(10000, size);
    }
    std::vector<double> temp(arr, arr + size);
    validateCoefficients(temp);
    coeffs_ = temp;
}

int Polynomial::degree() const noexcept {
    return static_cast<int>(coeffs_.size()) - 1;
}

int Polynomial::size() const noexcept {
    return static_cast<int>(coeffs_.size());
}

const std::vector<double>& Polynomial::coefficients() const noexcept {
    return coeffs_;
}

double Polynomial::evaluate(double x) const {
    if (!std::isfinite(x)) {
        throw std::invalid_argument("Evaluation point x must be finite");
    }
    double r = 0.0;
    for (int i = degree(); i >= 0; --i) {
        r = r * x + coeffs_[i];
    }
    return r;
}

Polynomial Polynomial::derivative() const {
    if (size() == 1) {
        return Polynomial({ 0.0 });
    }
    std::vector<double> out(size() - 1);
    for (int i = 1; i < size(); ++i) {
        out[i - 1] = i * coeffs_[i];
    }
    return Polynomial(out);
}

Polynomial Polynomial::integral(double constant) const {
    if (!std::isfinite(constant)) {
        throw std::invalid_argument("Integration constant must be finite");
    }
    std::vector<double> out(size() + 1);
    out[0] = constant;
    for (int i = 0; i < size(); ++i) {
        out[i + 1] = coeffs_[i] / (i + 1);
    }
    return Polynomial(out);
}

Polynomial Polynomial::add(const Polynomial& other) const {
    int na = size();
    int nb = other.size();
    int n = std::max(na, nb);
    std::vector<double> out(n, 0.0);
    for (int i = 0; i < n; ++i) {
        out[i] = (i < na ? coeffs_[i] : 0.0) + (i < nb ? other.coeffs_[i] : 0.0);
    }
    return Polynomial(out);
}

Polynomial Polynomial::multiply(const Polynomial& other) const {
    int na = size();
    int nb = other.size();
    int n = na + nb - 1;
    std::vector<double> out(n, 0.0);
    for (int i = 0; i < na; ++i) {
        for (int j = 0; j < nb; ++j) {
            double product = coeffs_[i] * other.coeffs_[j];
            out[i + j] += product;
            if (!std::isfinite(out[i + j])) {
                throw std::overflow_error("Polynomial multiplication overflow");
            }
        }
    }
    return Polynomial(out);
}

// Strong guarantee: валідація виконується до модифікації основного стану об'єкта
void Polynomial::setCoefficients(const std::vector<double>& newCoeffs) {
    validateCoefficients(newCoeffs);
    coeffs_ = newCoeffs;
}