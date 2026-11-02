#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <vector>
#include "Polynomial.h"
#include "PolynomialExceptions.h"

namespace {

    const double kNaN = std::numeric_limits<double>::quiet_NaN();
    const double kInf = std::numeric_limits<double>::infinity();

    // Допоміжна функція (уникає неоднозначності виклику конструкторів з {...})
    Polynomial P(std::initializer_list<double> c) {
        return Polynomial(std::vector<double>(c));
    }

    void ExpectInvariants(const Polynomial& p) {
        ASSERT_GE(p.size(), 1);
        EXPECT_LE(static_cast<std::size_t>(p.size()), Polynomial::kMaxCapacity);
        for (double c : p.coefficients()) EXPECT_TRUE(std::isfinite(c));
        if (p.size() > 1) EXPECT_NE(p.coefficients().back(), 0.0);
    }

} // namespace

// ---------- Конструктори та основні операції ----------

TEST(PolynomialConstruction, DefaultIsZeroPolynomial) {
    Polynomial p;
    EXPECT_EQ(p.size(), 1);
    EXPECT_EQ(p.degree(), 0);
    EXPECT_DOUBLE_EQ(p.getCoefficient(0), 0.0);
    ExpectInvariants(p);
}

TEST(PolynomialConstruction, VectorConstructorStoresCoefficients) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    EXPECT_EQ(p.size(), 3);
    EXPECT_EQ(p.degree(), 2);
    EXPECT_DOUBLE_EQ(p.getCoefficient(2), 3.0);
    ExpectInvariants(p);
}

TEST(PolynomialConstruction, ArrayConstructorStoresCoefficients) {
    std::array<double, 3> arr{ 4.0, 5.0, 6.0 };
    Polynomial p(arr.data(), 3);
    EXPECT_EQ(p.size(), 3);
    EXPECT_DOUBLE_EQ(p.getCoefficient(1), 5.0);
}

TEST(PolynomialConstruction, NormalizationRemovesTrailingZeros) {
    EXPECT_EQ(P({ 1.0, 0.0, 0.0 }).size(), 1);
    Polynomial zero = P({ 0.0, 0.0, 0.0 });
    EXPECT_EQ(zero.size(), 1);
    EXPECT_EQ(zero.degree(), 0);
}

// ---------- Сценарії помилок: конструктори ----------

TEST(PolynomialErrors, EmptyCoefficientsThrowInvalidArgument) {
    EXPECT_THROW(P({}), std::invalid_argument);
}

TEST(PolynomialErrors, NaNAndInfThrowInvalidCoefficient) {
    EXPECT_THROW(P({ 1.0, kNaN, 3.0 }), InvalidCoefficientException);
    EXPECT_THROW(P({ kInf }), InvalidCoefficientException);
    EXPECT_THROW(P({ 1.0, -kInf }), InvalidCoefficientException);
}

TEST(PolynomialErrors, InvalidCoefficientCarriesDiagnostics) {
    try {
        P({ 1.0, kNaN });
        FAIL() << "Expected InvalidCoefficientException";
    }
    catch (const InvalidCoefficientException& e) {
        EXPECT_EQ(e.parameter(), "coeffs[1]");
        EXPECT_TRUE(std::isnan(e.value()));
    }
}

TEST(PolynomialErrors, ArrayConstructorRejectsInvalidArguments) {
    std::array<double, 3> arr{ 1.0, 2.0, 3.0 };
    EXPECT_THROW(Polynomial(nullptr, 3), std::invalid_argument);
    EXPECT_THROW(Polynomial(arr.data(), 0), std::invalid_argument);
    EXPECT_THROW(Polynomial(arr.data(), -1), std::invalid_argument);

    std::vector<double> big(Polynomial::kMaxCapacity + 1, 1.0);
    EXPECT_THROW(Polynomial(big.data(), static_cast<int>(big.size())), PolynomialCapacityException);

    std::array<double, 2> bad{ 1.0, kNaN };
    EXPECT_THROW(Polynomial(bad.data(), 2), InvalidCoefficientException);
}

TEST(PolynomialBoundary, CapacityLimit) {
    EXPECT_NO_THROW(Polynomial(std::vector<double>(Polynomial::kMaxCapacity, 1.0)));
    try {
        Polynomial p(std::vector<double>(Polynomial::kMaxCapacity + 1, 1.0));
        FAIL() << "Expected PolynomialCapacityException";
    }
    catch (const PolynomialCapacityException& e) {
        EXPECT_EQ(e.capacity(), Polynomial::kMaxCapacity);
        EXPECT_EQ(e.requested(), Polynomial::kMaxCapacity + 1);
    }
}

TEST(PolynomialErrors, ExceptionHierarchyAllowsBaseCatch) {
    EXPECT_THROW(P({ kNaN }), PolynomialException);
    EXPECT_THROW(P({ kNaN }), std::runtime_error);
    EXPECT_THROW(P({ kNaN }), std::exception);
    EXPECT_THROW(Polynomial(std::vector<double>(Polynomial::kMaxCapacity + 1, 1.0)), PolynomialException);
}

// ---------- Доступ за індексом ----------

TEST(PolynomialAccess, GetAndSetCoefficient) {
    Polynomial p = P({ 5.0, 10.0, 15.0 });
    EXPECT_DOUBLE_EQ(p.getCoefficient(0), 5.0);
    EXPECT_DOUBLE_EQ(p.getCoefficient(2), 15.0);
    p.setCoefficient(1, 20.0);
    EXPECT_DOUBLE_EQ(p.getCoefficient(1), 20.0);
    ExpectInvariants(p);
}

TEST(PolynomialAccess, IndexOutOfRange) {
    Polynomial p = P({ 5.0, 10.0, 15.0 });
    EXPECT_THROW(p.getCoefficient(3), std::out_of_range);
    EXPECT_THROW(p.setCoefficient(3, 1.0), std::out_of_range);
    EXPECT_NO_THROW(p.getCoefficient(2));
}

TEST(PolynomialAccess, SetNaNThrowsInvalidCoefficient) {
    Polynomial p = P({ 1.0, 2.0 });
    EXPECT_THROW(p.setCoefficient(0, kNaN), InvalidCoefficientException);
    EXPECT_THROW(p.setCoefficient(1, kInf), InvalidCoefficientException);
}

TEST(PolynomialAccess, SettingLeadingCoefficientToZeroNormalizes) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    p.setCoefficient(2, 0.0);
    EXPECT_EQ(p.size(), 2);
    ExpectInvariants(p);
}

// ---------- Обчислення ----------

TEST(PolynomialMath, Evaluate) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    EXPECT_NEAR(p.evaluate(2.0), 17.0, 1e-9);
    EXPECT_NEAR(p.evaluate(0.0), 1.0, 1e-9);
    EXPECT_NEAR(p.evaluate(-1.0), 2.0, 1e-9);
}

TEST(PolynomialErrors, EvaluateRejectsInvalidParameter) {
    Polynomial p = P({ 1.0, 2.0 });
    EXPECT_THROW(p.evaluate(kNaN), std::invalid_argument);
    EXPECT_THROW(p.evaluate(kInf), std::invalid_argument);
}

TEST(PolynomialErrors, EvaluateOverflow) {
    EXPECT_THROW(P({ 1e308, 1e308 }).evaluate(10.0), std::overflow_error);
}

TEST(PolynomialMath, Derivative) {
    Polynomial d = P({ 2.0, 6.0, 3.0 }).derivative();   // 6 + 6x
    ASSERT_EQ(d.size(), 2);
    EXPECT_NEAR(d.getCoefficient(0), 6.0, 1e-9);
    EXPECT_NEAR(d.getCoefficient(1), 6.0, 1e-9);

    Polynomial dc = P({ 7.0 }).derivative();
    EXPECT_EQ(dc.size(), 1);
    EXPECT_NEAR(dc.getCoefficient(0), 0.0, 1e-9);
}

TEST(PolynomialErrors, DerivativeOverflow) {
    EXPECT_THROW(P({ 0.0, 0.0, 1e308 }).derivative(), std::overflow_error);
}

TEST(PolynomialMath, Integral) {
    Polynomial in = P({ 2.0, 6.0, 3.0 }).integral(1.0);  // 1 + 2x + 3x^2 + x^3
    ASSERT_EQ(in.size(), 4);
    EXPECT_NEAR(in.getCoefficient(0), 1.0, 1e-9);
    EXPECT_NEAR(in.getCoefficient(1), 2.0, 1e-9);
    EXPECT_NEAR(in.getCoefficient(2), 3.0, 1e-9);
    EXPECT_NEAR(in.getCoefficient(3), 1.0, 1e-9);
}

TEST(PolynomialErrors, IntegralInvalidParameterAndCapacity) {
    EXPECT_THROW(P({ 1.0 }).integral(kNaN), std::invalid_argument);
    EXPECT_THROW(P({ 1.0 }).integral(kInf), std::invalid_argument);
    Polynomial full(std::vector<double>(Polynomial::kMaxCapacity, 1.0));
    EXPECT_THROW(full.integral(), PolynomialCapacityException);
}

TEST(PolynomialMath, Add) {
    Polynomial s = P({ 1.0, 2.0 }).add(P({ 3.0, 4.0, 5.0 }));
    ASSERT_EQ(s.size(), 3);
    EXPECT_NEAR(s.getCoefficient(0), 4.0, 1e-9);
    EXPECT_NEAR(s.getCoefficient(1), 6.0, 1e-9);
    EXPECT_NEAR(s.getCoefficient(2), 5.0, 1e-9);
}

TEST(PolynomialMath, AddNormalizesCancelledLeadingTerm) {
    Polynomial s = P({ 1.0, 2.0 }).add(P({ 3.0, -2.0 }));
    EXPECT_EQ(s.size(), 1);
    EXPECT_NEAR(s.getCoefficient(0), 4.0, 1e-9);
}

TEST(PolynomialErrors, AddOverflow) {
    EXPECT_THROW(P({ 1e308 }).add(P({ 1e308 })), std::overflow_error);
}

TEST(PolynomialMath, Multiply) {
    Polynomial m = P({ 1.0, 2.0 }).multiply(P({ 3.0, 4.0 }));  // 3 + 10x + 8x^2
    ASSERT_EQ(m.size(), 3);
    EXPECT_NEAR(m.getCoefficient(0), 3.0, 1e-9);
    EXPECT_NEAR(m.getCoefficient(1), 10.0, 1e-9);
    EXPECT_NEAR(m.getCoefficient(2), 8.0, 1e-9);
}

TEST(PolynomialErrors, MultiplyOverflowAndCapacity) {
    EXPECT_THROW(P({ 1e200, 1.0 }).multiply(P({ 1e200, 1.0 })), std::overflow_error);
    Polynomial a(std::vector<double>(5001, 1.0));
    EXPECT_THROW(a.multiply(a), PolynomialCapacityException);
}

TEST(PolynomialMath, MultiplyAssign) {
    Polynomial p = P({ 1.0, 1.0 });
    p *= P({ 1.0, 1.0 });                                      // 1 + 2x + x^2
    ASSERT_EQ(p.size(), 3);
    EXPECT_NEAR(p.getCoefficient(1), 2.0, 1e-9);

    Polynomial q = P({ 1.0, 1.0 });
    q *= q;                                                    // самопомноження
    ASSERT_EQ(q.size(), 3);
    EXPECT_NEAR(q.getCoefficient(2), 1.0, 1e-9);
}

// ---------- Сильна гарантія: стан не змінюється після невдалої операції ----------

TEST(PolynomialStrongGuarantee, MultiplyAssignOverflowKeepsState) {
    Polynomial p = P({ 1.0, 2.0 });
    const std::vector<double> before = p.coefficients();
    EXPECT_THROW(p *= P({ 1e308, 1e308 }), std::overflow_error);
    EXPECT_EQ(p.coefficients(), before);
    ExpectInvariants(p);
}

TEST(PolynomialStrongGuarantee, MultiplyAssignCapacityKeepsState) {
    Polynomial p(std::vector<double>(5001, 1.0));
    const std::vector<double> before = p.coefficients();
    EXPECT_THROW(p *= p, PolynomialCapacityException);
    EXPECT_EQ(p.coefficients(), before);
}

TEST(PolynomialStrongGuarantee, SetCoefficientsFailureKeepsState) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    const std::vector<double> before = p.coefficients();

    EXPECT_THROW(p.setCoefficients({ 5.0, kNaN, 7.0 }), InvalidCoefficientException);
    EXPECT_EQ(p.coefficients(), before);

    EXPECT_THROW(p.setCoefficients({ 5.0, 6.0, kInf }), InvalidCoefficientException);
    EXPECT_EQ(p.coefficients(), before);

    EXPECT_THROW(p.setCoefficients({}), std::invalid_argument);
    EXPECT_EQ(p.coefficients(), before);

    EXPECT_THROW(p.setCoefficients(std::vector<double>(Polynomial::kMaxCapacity + 1, 1.0)),
        PolynomialCapacityException);
    EXPECT_EQ(p.coefficients(), before);
    ExpectInvariants(p);
}

TEST(PolynomialStrongGuarantee, SetCoefficientsSuccessReplacesAll) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    p.setCoefficients({ 9.0, 8.0 });
    EXPECT_EQ(p.size(), 2);
    EXPECT_DOUBLE_EQ(p.getCoefficient(0), 9.0);
}

TEST(PolynomialStrongGuarantee, SetCoefficientFailureKeepsState) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    const std::vector<double> before = p.coefficients();
    EXPECT_THROW(p.setCoefficient(10, 1.0), std::out_of_range);
    EXPECT_EQ(p.coefficients(), before);
    EXPECT_THROW(p.setCoefficient(1, kNaN), InvalidCoefficientException);
    EXPECT_EQ(p.coefficients(), before);
}

// ---------- Інваріанти після серії невдалих операцій ----------

TEST(PolynomialInvariants, HoldAfterFailedOperations) {
    Polynomial p = P({ 1.0, 2.0, 3.0 });
    EXPECT_THROW(p.setCoefficient(9, 1.0), std::out_of_range);
    EXPECT_THROW(p.setCoefficients({ kNaN }), InvalidCoefficientException);
    EXPECT_THROW(p *= P({ 1e308, 1e308, 1e308 }), std::overflow_error);
    ExpectInvariants(p);
    EXPECT_EQ(p.size(), 3);
}