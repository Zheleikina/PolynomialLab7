cmake_minimum_required(VERSION 3.21)
project(PolynomialLab VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Google Test встановлюється через vcpkg (vcpkg.json + CMAKE_TOOLCHAIN_FILE)
find_package(GTest CONFIG REQUIRED)

# Статична бібліотека
add_library(PolynomialLib STATIC src/Polynomial.cpp)
target_include_directories(PolynomialLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Демонстраційна програма
add_executable(PolynomialDemo demo/main.cpp)
target_link_libraries(PolynomialDemo PRIVATE PolynomialLib)

# Демонстрація некоректних підходів до винятків
add_executable(AntipatternDemo antipattern/main.cpp)

# Модульні тести
enable_testing()
add_executable(PolynomialTests tests/PolynomialTests.cpp)
target_link_libraries(PolynomialTests PRIVATE PolynomialLib GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(PolynomialTests)