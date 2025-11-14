#include "money.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>

// Конструктор по умолчанию
Money::Money() : digits(nullptr), size(0), is_negative(false) {
    digits = new unsigned char[1];
    digits[0] = 0;
    size = 1;
}

// Конструктор из строки
Money::Money(const std::string& amount) : digits(nullptr), size(0), is_negative(false) {
    if (amount.empty()) {
        throw std::invalid_argument("Empty string cannot be converted to Money");
    }

    std::string processed = amount;
    
    // Обработка знака
    if (processed[0] == '-') {
        is_negative = true;
        processed = processed.substr(1);
    } else if (processed[0] == '+') {
        processed = processed.substr(1);
    }

    // Проверка формата
    bool has_decimal = false;
    size_t decimal_pos = std::string::npos;
    
    for (size_t i = 0; i < processed.length(); ++i) {
        if (processed[i] == '.' || processed[i] == ',') {
            if (has_decimal) {
                throw std::invalid_argument("Invalid money format: multiple decimal separators");
            }
            has_decimal = true;
            decimal_pos = i;
        } else if (!std::isdigit(processed[i])) {
            throw std::invalid_argument("Invalid character in money string");
        }
    }

    // Обработка копеек
    std::string rubles_str, kopecks_str;
    
    if (has_decimal) {
        rubles_str = processed.substr(0, decimal_pos);
        kopecks_str = processed.substr(decimal_pos + 1);
        
        // Нормализация копеек до 2 знаков
        if (kopecks_str.length() > 2) {
            kopecks_str = kopecks_str.substr(0, 2);
        } else if (kopecks_str.length() < 2) {
            kopecks_str.append(2 - kopecks_str.length(), '0');
        }
    } else {
        rubles_str = processed;
        kopecks_str = "00";
    }

    // Удаление ведущих нулей у рублей
    size_t start = 0;
    while (start < rubles_str.length() - 1 && rubles_str[start] == '0') {
        start++;
    }
    rubles_str = rubles_str.substr(start);

    // Объединение цифр (копейки идут первыми в младших разрядах)
    std::string all_digits = kopecks_str + rubles_str;
    
    if (all_digits.empty() || (all_digits.length() == 1 && all_digits[0] == '0')) {
        // Нулевая сумма
        digits = new unsigned char[1];
        digits[0] = 0;
        size = 1;
        is_negative = false;
        return;
    }

    // Создание массива цифр
    size = all_digits.length();
    digits = new unsigned char[size];
    
    for (size_t i = 0; i < size; ++i) {
        digits[i] = all_digits[size - 1 - i] - '0';  // Младшая цифра сначала
    }

    removeLeadingZeros();
}

// Конструктор копирования
Money::Money(const Money& other) 
    : size(other.size), is_negative(other.is_negative) {
    digits = new unsigned char[size];
    for (size_t i = 0; i < size; ++i) {
        digits[i] = other.digits[i];
    }
}

// Конструктор перемещения
Money::Money(Money&& other) noexcept 
    : digits(other.digits), size(other.size), is_negative(other.is_negative) {
    other.digits = nullptr;
    other.size = 0;
    other.is_negative = false;
}

// Деструктор
Money::~Money() {
    delete[] digits;
}

// Оператор присваивания копированием
Money& Money::operator=(const Money& other) {
    if (this != &other) {
        delete[] digits;
        
        size = other.size;
        is_negative = other.is_negative;
        digits = new unsigned char[size];
        
        for (size_t i = 0; i < size; ++i) {
            digits[i] = other.digits[i];
        }
    }
    return *this;
}

// Оператор присваивания перемещением
Money& Money::operator=(Money&& other) noexcept {
    if (this != &other) {
        delete[] digits;
        
        digits = other.digits;
        size = other.size;
        is_negative = other.is_negative;
        
        other.digits = nullptr;
        other.size = 0;
        other.is_negative = false;
    }
    return *this;
}

// Удаление ведущих нулей
void Money::removeLeadingZeros() {
    if (size == 0) return;
    
    size_t new_size = size;
    while (new_size > 1 && digits[new_size - 1] == 0) {
        new_size--;
    }
    
    if (new_size != size) {
        unsigned char* new_digits = new unsigned char[new_size];
        for (size_t i = 0; i < new_size; ++i) {
            new_digits[i] = digits[i];
        }
        delete[] digits;
        digits = new_digits;
        size = new_size;
    }
    
    // Если остался только ноль, сбрасываем знак
    if (size == 1 && digits[0] == 0) {
        is_negative = false;
    }
}

// Сравнение абсолютных значений
int Money::compareAbsolute(const Money& other) const {
    if (size > other.size) return 1;
    if (size < other.size) return -1;
    
    for (int i = size - 1; i >= 0; --i) {
        if (digits[i] > other.digits[i]) return 1;
        if (digits[i] < other.digits[i]) return -1;
    }
    
    return 0;
}

// Сложение абсолютных значений
Money Money::addAbsolute(const Money& other) const {
    size_t max_size = std::max(size, other.size);
    unsigned char* result_digits = new unsigned char[max_size + 1]();
    
    int carry = 0;
    for (size_t i = 0; i < max_size; ++i) {
        int sum = carry;
        if (i < size) sum += digits[i];
        if (i < other.size) sum += other.digits[i];
        
        result_digits[i] = sum % 10;
        carry = sum / 10;
    }
    
    if (carry > 0) {
        result_digits[max_size] = carry;
    }
    
    Money result;
    delete[] result.digits;
    result.digits = result_digits;
    result.size = carry > 0 ? max_size + 1 : max_size;
    result.removeLeadingZeros();
    
    return result;
}

// Вычитание абсолютных значений (this >= other)
Money Money::subtractAbsolute(const Money& other) const {
    unsigned char* result_digits = new unsigned char[size];
    int borrow = 0;
    
    for (size_t i = 0; i < size; ++i) {
        int diff = digits[i] - borrow;
        if (i < other.size) {
            diff -= other.digits[i];
        }
        
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        result_digits[i] = diff;
    }
    
    if (borrow != 0) {
        delete[] result_digits;
        throw std::logic_error("Absolute subtraction resulted in negative");
    }
    
    Money result;
    delete[] result.digits;
    result.digits = result_digits;
    result.size = size;
    result.removeLeadingZeros();
    
    return result;
}

// Сложение
Money Money::add(const Money& other) const {
    if (is_negative == other.is_negative) {
        // Одинаковые знаки - складываем абсолютные значения
        Money result = addAbsolute(other);
        result.is_negative = is_negative;
        return result;
    } else {
        // Разные знаки - вычитаем абсолютные значения
        int abs_compare = compareAbsolute(other);
        
        if (abs_compare == 0) {
            // Абсолютные значения равны - результат ноль
            return Money();
        } else if (abs_compare > 0) {
            // |this| > |other|
            Money result = subtractAbsolute(other);
            result.is_negative = is_negative;
            return result;
        } else {
            // |this| < |other|
            Money result = other.subtractAbsolute(*this);
            result.is_negative = other.is_negative;
            return result;
        }
    }
}

// Вычитание
Money Money::subtract(const Money& other) const {
    if (is_negative != other.is_negative) {
        // Разные знаки - складываем абсолютные значения
        Money result = addAbsolute(other);
        result.is_negative = is_negative;
        return result;
    } else {
        // Одинаковые знаки - вычитаем абсолютные значения
        int abs_compare = compareAbsolute(other);
        
        if (abs_compare == 0) {
            // Абсолютные значения равны - результат ноль
            return Money();
        } else if (abs_compare > 0) {
            // |this| > |other|
            Money result = subtractAbsolute(other);
            result.is_negative = is_negative;
            return result;
        } else {
            // |this| < |other|
            Money result = other.subtractAbsolute(*this);
            result.is_negative = !is_negative;
            return result;
        }
    }
}

// Умножение на целое число
Money Money::multiply(int multiplier) const {
    if (multiplier == 0) {
        return Money();
    }
    
    if (multiplier < 0) {
        Money result = multiply(-multiplier);
        result.is_negative = !is_negative;
        return result;
    }
    
    unsigned char* result_digits = new unsigned char[size + 10](); // Запас для умножения
    int carry = 0;
    
    for (size_t i = 0; i < size; ++i) {
        int product = digits[i] * multiplier + carry;
        result_digits[i] = product % 10;
        carry = product / 10;
    }
    
    size_t result_size = size;
    while (carry > 0) {
        result_digits[result_size] = carry % 10;
        carry /= 10;
        result_size++;
    }
    
    Money result;
    delete[] result.digits;
    result.digits = result_digits;
    result.size = result_size;
    result.is_negative = is_negative;
    result.removeLeadingZeros();
    
    return result;
}

// Деление на целое число
Money Money::divide(int divisor) const {
    if (divisor == 0) {
        throw std::invalid_argument("Division by zero");
    }
    
    if (isZero()) {
        return Money();
    }
    
    bool result_negative = is_negative != (divisor < 0);
    divisor = std::abs(divisor);
    
    // Деление с сохранением 2 знаков после запятой (копеек)
    unsigned char* result_digits = new unsigned char[size + 2](); // +2 для дополнительных разрядов
    int remainder = 0;
    
    // Деление с конца (со старших разрядов)
    for (int i = size - 1; i >= 0; --i) {
        int current = remainder * 10 + digits[i];
        result_digits[i] = current / divisor;
        remainder = current % divisor;
    }
    
    // Обработка остатка для копеек
    for (int i = 0; i < 2; ++i) {
        remainder *= 10;
        result_digits[size + i] = remainder / divisor;
        remainder %= divisor;
    }
    
    // Округление
    if (remainder * 2 >= divisor) {
        // Округление вверх
        int carry = 1;
        for (size_t i = 0; i < size + 2 && carry > 0; ++i) {
            int sum = result_digits[i] + carry;
            result_digits[i] = sum % 10;
            carry = sum / 10;
        }
    }
    
    Money result;
    delete[] result.digits;
    result.digits = result_digits;
    result.size = size + 2;
    result.is_negative = result_negative;
    result.removeLeadingZeros();
    
    return result;
}

// Операции сравнения
bool Money::equals(const Money& other) const {
    if (is_negative != other.is_negative || size != other.size) {
        return false;
    }
    
    for (size_t i = 0; i < size; ++i) {
        if (digits[i] != other.digits[i]) {
            return false;
        }
    }
    
    return true;
}

bool Money::lessThan(const Money& other) const {
    if (is_negative != other.is_negative) {
        return is_negative;
    }
    
    int abs_compare = compareAbsolute(other);
    
    if (is_negative) {
        return abs_compare > 0;
    } else {
        return abs_compare < 0;
    }
}

bool Money::greaterThan(const Money& other) const {
    return !equals(other) && !lessThan(other);
}

// Преобразование в строку
std::string Money::toString() const {
    if (size == 0 || (size == 1 && digits[0] == 0)) {
        return "0.00";
    }
    
    std::stringstream ss;
    
    if (is_negative) {
        ss << "-";
    }
    
    // Рубли (все цифры кроме последних двух)
    bool has_rubles = false;
    for (int i = size - 1; i >= 2; --i) {
        ss << static_cast<char>(digits[i] + '0');
        has_rubles = true;
    }
    
    if (!has_rubles) {
        ss << "0";
    }
    
    ss << ".";
    
    // Копейки (последние две цифры)
    if (size >= 2) {
        ss << static_cast<char>(digits[1] + '0') 
           << static_cast<char>(digits[0] + '0');
    } else if (size == 1) {
        ss << "0" << static_cast<char>(digits[0] + '0');
    }
    
    return ss.str();
}

// Вспомогательные методы
bool Money::isNegative() const {
    return is_negative;
}

bool Money::isZero() const {
    return size == 1 && digits[0] == 0;
}

size_t Money::getSize() const {
    return size;
}

const unsigned char* Money::getDigits() const {
    return digits;
}