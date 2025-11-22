#ifndef MONEY_H
#define MONEY_H

#include <stdexcept>
#include <string>

class Money {
private:
    unsigned char* digits;  // Динамический массив цифр
    size_t size;           // Размер массива
    bool is_negative;      // Флаг отрицательности

    // Вспомогательные методы
    void removeLeadingZeros();
    int compareAbsolute(const Money& other) const;
    Money addAbsolute(const Money& other) const;
    Money subtractAbsolute(const Money& other) const;

public:
    // Конструкторы
    Money();
    explicit Money(const std::string& amount);
    Money(const Money& other);
    Money(Money&& other) noexcept;
    
    // Деструктор
    ~Money();

    // Арифметические операции с присваиванием
    Money add(const Money& other) const;
    Money subtract(const Money& other) const;
    Money multiply(int multiplier) const;
    Money divide(int divisor) const;

    // Операции сравнения
    bool equals(const Money& other) const;
    bool lessThan(const Money& other) const;
    bool greaterThan(const Money& other) const;

    // Вспомогательные методы
    std::string toString() const;
    bool isNegative() const;
    bool isZero() const;

    // Геттеры
    size_t getSize() const;
    const unsigned char* getDigits() const;
};

#endif // MONEY_H