#include "Polynomial.h"
#include "PolynomialExceptions.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

void Polynomial::validateCoefficients(const std::vector<double>& coeffs) {
    if (coeffs.empty()) {
        throw std::invalid_argument("Polynomial coefficient list cannot be empty");
    }
    if (coeffs.size() > kMaxCapacity) {
        throw PolynomialCapacityException(kMaxCapacity, coeffs.size());
    }
    for (std::size_t i = 0; i < coeffs.size(); ++i) {
        if (!std::isfinite(coeffs[i])) {
            throw InvalidCoefficientException("coeffs[" + std::to_string(i) + "]", coeffs[i]);
        }
    }
}

void Polynomial::normalize(std::vector<double>& v) noexcept {
    while (v.size() > 1 && v.back() == 0.0) {
        v.pop_back();
    }
}

Polynomial::Polynomial() : coeffs_{ 0.0 } {}

Polynomial::Polynomial(const std::vector<double>& coeffs) {
    validateCoefficients(coeffs);
    std::vector<double> temp = coeffs;
    normalize(temp);
    coeffs_ = std::move(temp);
}

Polynomial::Polynomial(const double* arr, int size) {
    if (!arr) {
        throw std::invalid_argument("Pointer to coefficients cannot be nullptr");
    }
    if (size <= 0) {
        throw std::invalid_argument("Polynomial size must be greater than zero");
    }
    if (static_cast<std::size_t>(size) > kMaxCapacity) {
        throw PolynomialCapacityException(kMaxCapacity, static_cast<std::size_t>(size));
    }
    std::vector<double> temp(arr, arr + size);
    validateCoefficients(temp);
    normalize(temp);
    coeffs_ = std::move(temp);
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

double Polynomial::getCoefficient(std::size_t index) const {
    if (index >= coeffs_.size()) {
        throw std::out_of_range("Coefficient index out of range");
    }
    return coeffs_[index];
}

// Strong: усі перевірки і зміни виконуються в temp, commit - noexcept swap
void Polynomial::setCoefficient(std::size_t index, double value) {
    if (index >= coeffs_.size()) {
        throw std::out_of_range("Coefficient index out of range");
    }
    if (!std::isfinite(value)) {
        throw InvalidCoefficientException("coeffs[" + std::to_string(index) + "]", value);
    }
    std::vector<double> temp = coeffs_;
    temp[index] = value;
    normalize(temp);
    coeffs_.swap(temp);
}

double Polynomial::evaluate(double x) const {
    if (!std::isfinite(x)) {
        throw std::invalid_argument("Evaluation point x must be finite");
    }
    double r = 0.0;
    for (std::size_t i = coeffs_.size(); i-- > 0;) {
        r = r * x + coeffs_[i];
        if (!std::isfinite(r)) {
            throw std::overflow_error("Polynomial evaluation overflow");
        }
    }
    return r;
}

Polynomial Polynomial::derivative() const {
    if (coeffs_.size() == 1) {
        return Polynomial(std::vector<double>{ 0.0 });
    }
    std::vector<double> out(coeffs_.size() - 1);
    for (std::size_t i = 1; i < coeffs_.size(); ++i) {
        out[i - 1] = static_cast<double>(i) * coeffs_[i];
        if (!std::isfinite(out[i - 1])) {
            throw std::overflow_error("Polynomial derivative overflow");
        }
    }
    return Polynomial(out);
}

Polynomial Polynomial::integral(double constant) const {
    if (!std::isfinite(constant)) {
        throw std::invalid_argument("Integration constant must be finite");
    }
    if (coeffs_.size() + 1 > kMaxCapacity) {
        throw PolynomialCapacityException(kMaxCapacity, coeffs_.size() + 1);
    }
    std::vector<double> out(coeffs_.size() + 1);
    out[0] = constant;
    for (std::size_t i = 0; i < coeffs_.size(); ++i) {
        // ділення скінченного числа на (i+1) >= 1 не може переповнити double
        out[i + 1] = coeffs_[i] / static_cast<double>(i + 1);
    }
    return Polynomial(out);
}

Polynomial Polynomial::add(const Polynomial& other) const {
    const std::size_t na = coeffs_.size();
    const std::size_t nb = other.coeffs_.size();
    const std::size_t n = std::max(na, nb);
    std::vector<double> out(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        out[i] = (i < na ? coeffs_[i] : 0.0) + (i < nb ? other.coeffs_[i] : 0.0);
        if (!std::isfinite(out[i])) {
            throw std::overflow_error("Polynomial addition overflow");
        }
    }
    return Polynomial(out);
}

Polynomial Polynomial::multiply(const Polynomial& other) const {
    const std::size_t na = coeffs_.size();
    const std::size_t nb = other.coeffs_.size();
    const std::size_t n = na + nb - 1;
    if (n > kMaxCapacity) {
        throw PolynomialCapacityException(kMaxCapacity, n);
    }
    std::vector<double> out(n, 0.0);
    for (std::size_t i = 0; i < na; ++i) {
        for (std::size_t j = 0; j < nb; ++j) {
            const double product = coeffs_[i] * other.coeffs_[j];
            if (!std::isfinite(product)) {
                throw std::overflow_error("Polynomial multiplication overflow");
            }
            out[i + j] += product;
            if (!std::isfinite(out[i + j])) {
                throw std::overflow_error("Polynomial multiplication overflow");
            }
        }
    }
    return Polynomial(out);
}

// Strong: prepare (multiply у тимчасовий об'єкт) -> commit (noexcept swap)
Polynomial& Polynomial::operator*=(const Polynomial& other) {
    Polynomial temp = this->multiply(other);
    coeffs_.swap(temp.coeffs_);
    return *this;
}

// Strong: validate -> copy -> normalize -> noexcept swap
void Polynomial::setCoefficients(const std::vector<double>& newCoeffs) {
    validateCoefficients(newCoeffs);
    std::vector<double> temp = newCoeffs;
    normalize(temp);
    coeffs_.swap(temp);
}