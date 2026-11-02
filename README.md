**Alina Zheleikina FIT 3-9**

lab7

# PolynomialLab - Бібліотека поліномів із забезпеченням безпеки винятків

Проєкт реалізує клас `Polynomial` на базі C++20 з повною підтримкою ідіоми RAII, власною ієрархією винятків та сильною гарантією безпеки винятків (Strong Exception Guarantee) для Варіанта 19.

## Інваріанти класу
1. Внутрішній вектор коефіцієнтів `coeffs_` ніколи не буває порожнім.
2. Усі коефіцієнти є скінченними числами (`std::isfinite == true`).
3. Розмір полінома не перевищує обмеження місткості (`kMaxCapacity = 10000`).

## Ієрархія винятків
- `PolynomialException` (базовий клас, успадкований від `std::runtime_error`)
  - `InvalidCoefficientException` (недійсний або нескінченний коефіцієнт)
  - `PolynomialCapacityException` (перевищення ліміту місткості)

## Гарантії безпеки винятків
- **Strong Guarantee (All-or-nothing):** Реалізована в методі `setCoefficients()`, `setCoefficient()` та `operator*=` за допомогою паттерна *Copy-and-Swap*. Якщо під час перевірки чи обчислень виникає виняток, початковий стан об'єкта залишається повністю незмінним.
- **Basic Guarantee:** Для методів обчислень (`evaluate`, `derivative`, `integral`).

## Збирання та запуск
1. Зберіть проєкт за допомогою CMake:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
