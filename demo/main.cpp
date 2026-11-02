#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include "Polynomial.h"
#include "PolynomialExceptions.h"
#include "ScopeLogger.h"

namespace {

    void print(const char* label, const Polynomial& p) {
        std::cout << label << " [size=" << p.size() << "]: ";
        for (double c : p.coefficients()) std::cout << c << ' ';
        std::cout << '\n';
    }

    // Рівень 3: виникнення винятку
    void level3() {
        ScopeLogger logger("level3_scope");
        std::cout << "[level3] Creating polynomial with NaN coefficient...\n";
        Polynomial p(std::vector<double>{ 1.0, 2.0, std::numeric_limits<double>::quiet_NaN() });
    }

    // Рівень 2: перехоплення, діагностика і повторне генерування через throw;
    void level2() {
        ScopeLogger logger("level2_scope");
        try {
            level3();
        }
        catch (const PolynomialException& ex) {
            std::cout << "[level2 LOG] Caught PolynomialException: " << ex.what()
                << " -> rethrowing with 'throw;'\n";
            throw;
        }
    }

    // Рівень 1: проміжний виклик
    void level1() {
        ScopeLogger logger("level1_scope");
        level2();
    }

    void demoStackUnwinding() {
        std::cout << "=== Part 1: stack unwinding and rethrow ===\n";
        try {
            level1();
        }
        // Від найбільш спеціалізованого типу до загального
        catch (const InvalidCoefficientException& ex) {
            std::cout << "[MAIN CATCH] InvalidCoefficientException: " << ex.what()
                << " | parameter = " << ex.parameter() << '\n';
        }
        catch (const PolynomialException& ex) {
            std::cout << "[MAIN CATCH] PolynomialException: " << ex.what() << '\n';
        }
        catch (const std::exception& ex) {
            std::cout << "[MAIN CATCH] std::exception: " << ex.what() << '\n';
        }
        catch (...) {
            std::cout << "[MAIN CATCH] unknown exception\n";
        }
        std::cout << "Program continues after exception handling.\n\n";
    }

    void demoErrorScenarios() {
        std::cout << "=== Part 2: error scenarios (variant 19) ===\n";
        const double nan = std::numeric_limits<double>::quiet_NaN();

        // Сценарій 1: порожній набір коефіцієнтів
        try {
            std::cout << "\n[Scenario 1] Empty coefficient set\n";
            Polynomial p(std::vector<double>{});
        }
        catch (const std::invalid_argument& e) {
            std::cout << "-> invalid_argument: " << e.what() << '\n';
        }

        // Сценарій 2: NaN/Inf
        try {
            std::cout << "\n[Scenario 2] NaN coefficient\n";
            Polynomial p(std::vector<double>{ 1.0, nan });
        }
        catch (const InvalidCoefficientException& e) {
            std::cout << "-> InvalidCoefficientException: " << e.what() << '\n';
        }
        catch (const PolynomialException& e) {
            std::cout << "-> PolynomialException: " << e.what() << '\n';
        }

        // Сценарій 3: некоректний індекс
        try {
            std::cout << "\n[Scenario 3] Coefficient index out of range\n";
            Polynomial p(std::vector<double>{ 1.0, 2.0 });
            p.getCoefficient(5);
        }
        catch (const std::out_of_range& e) {
            std::cout << "-> out_of_range: " << e.what() << '\n';
        }

        // Сценарій 4: переповнення при множенні + сильна гарантія
        {
            std::cout << "\n[Scenario 4] Multiplication overflow, strong guarantee of operator*=\n";
            Polynomial a(std::vector<double>{ 1.0, 2.0 });
            Polynomial b(std::vector<double>{ 1e308, 1e308 });
            print("Before", a);
            try {
                a *= b;
            }
            catch (const std::overflow_error& e) {
                std::cout << "-> overflow_error: " << e.what() << '\n';
            }
            print("After ", a);
        }

        // Сценарій 5: некоректний параметр операції
        try {
            std::cout << "\n[Scenario 5] Invalid operation parameter (NaN in evaluate)\n";
            Polynomial p(std::vector<double>{ 1.0, 2.0 });
            p.evaluate(nan);
        }
        catch (const std::invalid_argument& e) {
            std::cout << "-> invalid_argument: " << e.what() << '\n';
        }

        // Додатково: перевищення місткості
        try {
            std::cout << "\n[Extra] Capacity exceeded\n";
            Polynomial p(std::vector<double>(Polynomial::kMaxCapacity + 1, 1.0));
        }
        catch (const PolynomialCapacityException& e) {
            std::cout << "-> PolynomialCapacityException: " << e.what()
                << " (capacity=" << e.capacity() << ", requested=" << e.requested() << ")\n";
        }
    }

} // namespace

int main() {
    std::cout << "=== POLYNOMIAL LAB DEMO (variant 19) ===\n\n";
    demoStackUnwinding();
    demoErrorScenarios();
    std::cout << "\n=== DEMO FINISHED ===\n";
    return 0;
}