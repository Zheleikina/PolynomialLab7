#include <gtest/gtest.h>
#include "Polynomial.h"
#include "PolynomialExceptions.h"
#include <limits>

TEST(PolynomialTest, DefaultAndVectorConstructor) {
    Polynomial p1;
    EXPECT_EQ(p1.size(), 1);
    EXPECT_EQ(p1.degree(), 0);
    EXPECT_EQ(p1.evaluate(5.0), 0.0);

    Polynomial p2({ 1.0, 2.0, 3.0 }); // 1 + 2x + 3x^2
    EXPECT_EQ(p2.size(), 3);
    EXPECT_EQ(p2.degree(), 2);
    EXPECT_NEAR(p2.evaluate(2.0), 17.0, 1e-7); // 1 + 2(2) + 3(4) = 17
}

TEST(PolynomialTest, EvaluationExceptions) {
    Polynomial p({ 1.0, 2.0, 3.0 });
    EXPECT_THROW(p.evaluate(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
}

TEST(PolynomialTest, InvalidCoefficientsException) {
    double nanVal = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(Polynomial({ 1.0, nanVal, 3.0 }), InvalidCoefficientException);
    EXPECT_THROW(Polynomial(std::vector<double>{}), std::invalid_argument);
}

TEST(PolynomialTest, CapacityException) {
    std::vector<double> largeVec(15000, 1.0);
    EXPECT_THROW(Polynomial p(largeVec), PolynomialCapacityException);
}

TEST(PolynomialTest, ArithmeticOperations) {
    Polynomial p1({ 1.0, 2.0 });
    Polynomial p2({ 3.0, 4.0 });
    Polynomial sum = p1.add(p2);
    EXPECT_EQ(sum.size(), 2);
    EXPECT_NEAR(sum.evaluate(1.0), 10.0, 1e-7);

    Polynomial prod = p1.multiply(p2);
    EXPECT_NEAR(prod.evaluate(2.0), 3.0 + 10.0 * 2.0 + 8.0 * 4.0, 1e-7);
}

// Перевірка сильної гарантії безпеки: стан об'єкта не змінюється після невдалої операції
TEST(PolynomialTest, StrongGuaranteeStatePreservation) {
    Polynomial p({ 1.0, 2.0, 3.0 });
    std::vector<double> originalCoeffs = p.coefficients();

    double nanVal = std::numeric_limits<double>::quiet_NaN();
    EXPECT_THROW(p.setCoefficients({ 5.0, nanVal, 7.0 }), InvalidCoefficientException);

    EXPECT_EQ(p.size(), originalCoeffs.size());
    for (size_t i = 0; i < originalCoeffs.size(); ++i) {
        EXPECT_NEAR(p.coefficients()[i], originalCoeffs[i], 1e-7);
    }
}