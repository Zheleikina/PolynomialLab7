#include <iostream>
#include <exception>
#include "Polynomial.h"
#include "PolynomialExceptions.h"
#include "ScopeLogger.h"
#include <limits>

// Рівень 3: виникнення винятку
void level3() {
    ScopeLogger logger("level3_scope");
    std::cout << "[level3] Creating polynomial with invalid coefficient (NaN)...\n";
    Polynomial p({ 1.0, 2.0, std::numeric_limits<double>::quiet_NaN() });
}

// Рівень 2: перехоплення, логування та повторне поширення через throw;
void level2() {
    ScopeLogger logger("level2_scope");
    try {
        level3();
    }
    catch (const PolynomialException& ex) {
        std::cout << "[level2 LOG] Caught custom polynomial exception: " << ex.what() << '\n';
        throw; // Повторне генерування того ж винятку без object slicing
    }
}

// Рівень 1: проміжний виклик
void level1() {
    ScopeLogger logger("level1_scope");
    level2();
}

int main() {
    std::cout << "=== DEMO: Stack Unwinding & RAII ===\n\n";
    try {
        level1();
    }
    catch (const std::exception& ex) {
        std::cout << "[MAIN CATCH] Successfully caught final exception: " << ex.what() << '\n';
    }

    std::cout << "\nProgram finished normally after exception handling.\n";
    return 0;
}