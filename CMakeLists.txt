cmake_minimum_required(VERSION 3.21)
project(PolynomialLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Статична бібліотека поліномів
add_library(PolynomialLib STATIC
    src/Polynomial.cpp
)

target_include_directories(PolynomialLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Демонстраційна програма
add_executable(PolynomialDemo
    demo/main.cpp
)
target_link_libraries(PolynomialDemo
    PRIVATE PolynomialLib
)

# Демонстрація антипатернів
add_executable(AntipatternDemo
    antipattern/main.cpp
)
target_link_libraries(AntipatternDemo
    PRIVATE PolynomialLib
)

# --- АВТОМАТИЧНЕ ПІДКЛЮЧЕННЯ GOOGLE TEST ЧЕРЕЗ FETCHCONTENT ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Забороняємо Google Test встановлювати власні глобальні налаштування (для сумісності з MSVC)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(PolynomialTests
    tests/PolynomialTests.cpp
)

target_link_libraries(PolynomialTests
    PRIVATE
    PolynomialLib
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(PolynomialTests)