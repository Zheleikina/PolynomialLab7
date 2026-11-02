**Alina Zheleikina FIT 3-9**

lab7

# PolynomialLab — Лабораторна робота №7 (варіант 19)
**Автор:** Желейкіна Аліна Сергіївна, група ФІТ 3-9  
Статична бібліотека поліномів на C++20: власна ієрархія винятків, RAII, гарантії безпеки винятків, модульні тести Google Test (vcpkg + CMake + CTest).

## 1. Інваріанти класу `Polynomial`
1. Вектор коефіцієнтів `coeffs_` ніколи не порожній.
2. Усі коефіцієнти скінченні (`std::isfinite`).
3. Розмір не перевищує `kMaxCapacity = 10000`.
4. Поліном нормалізований: старший коефіцієнт ненульовий (крім нульового полінома `{0}`).

Інваріанти забезпечуються перевіркою у конструкторах і в методах зміни стану; некоректний стан недоступний через публічний інтерфейс.

## 2. Ієрархія винятків
- `PolynomialException` (← `std::runtime_error`)
  - `InvalidCoefficientException` (параметр, значення)
  - `PolynomialCapacityException` (ліміт, запитаний розмір)
- Стандартні: `std::invalid_argument`, `std::out_of_range`, `std::overflow_error`.

| Операція | Умова помилки | Виняток |
|---|---|---|
| Конструктори, `setCoefficients` | порожній набір | `std::invalid_argument` |
| Конструктор `(ptr, size)` | `nullptr` або `size <= 0` | `std::invalid_argument` |
| Конструктори, `setCoefficients`, `setCoefficient` | NaN/Inf | `InvalidCoefficientException` |
| Конструктори, `multiply`, `*=`, `integral` | розмір > 10000 | `PolynomialCapacityException` |
| `getCoefficient`, `setCoefficient` | індекс ≥ size | `std::out_of_range` |
| `multiply`, `*=`, `add`, `derivative`, `evaluate` | переповнення double | `std::overflow_error` |
| `evaluate`, `integral` | параметр NaN/Inf | `std::invalid_argument` |

## 3. Гарантії безпеки винятків
- **Strong:** `setCoefficient`, `setCoefficients`, `operator*=` (prepare-then-commit: усі обчислення в тимчасовому об'єкті, фіксація через `noexcept` `swap`). Конструктори теж strong: при винятку об'єкт не створюється.
- **Const-методи** (`getCoefficient`, `evaluate`, `derivative`, `integral`, `add`, `multiply`) не змінюють стан об'єкта, тому стан зберігається за будь-яких винятків.
- **No-throw:** `degree`, `size`, `coefficients` (`noexcept`).

**Фрагмент реалізації сильної гарантії:**
```cpp
Polynomial& Polynomial::operator*=(const Polynomial& other) {
    Polynomial temp = this->multiply(other);   // prepare: може кинути виняток
    coeffs_.swap(temp.coeffs_);                // commit: noexcept
    return *this;
}
