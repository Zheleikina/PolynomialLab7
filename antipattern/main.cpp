#include <iostream>
#include <string>
#include <exception>

void triggerPrimitiveException(int choice) {
    if (choice == 1) {
        throw 404; // Генеруємо int
    }
    else if (choice == 2) {
        throw std::string("Critical string error message!"); // Генеруємо std::string
    }
}

int main() {
    std::cout << "=== ANTIPATTERN DEMO ===\n";

    // 1. Спроба зловити int через catch (const std::exception&)
    try {
        std::cout << "Triggering int exception...\n";
        triggerPrimitiveException(1);
    }
    catch (const std::exception& e) {
        std::cout << "[CAUGHT std::exception]: " << e.what() << '\n';
    }
    catch (...) {
        std::cout << "[CAUGHT catch (...)] Successfully intercepted non-standard exception (int)!\n";
    }

    // 2. Спроба зловити std::string через catch (const std::exception&)
    try {
        std::cout << "\nTriggering std::string exception...\n";
        triggerPrimitiveException(2);
    }
    catch (const std::exception& e) {
        std::cout << "[CAUGHT std::exception]: " << e.what() << '\n';
    }
    catch (...) {
        std::cout << "[CAUGHT catch (...)] Successfully intercepted non-standard exception (std::string)!\n";
    }

    std::cout << "\nConclusion: Standard polymorphic exception handlers require derivation from std::exception.\n";
    return 0;
}