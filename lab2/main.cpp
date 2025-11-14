#include "money.h"
#include <iostream>

int main() {
    try {
        // Создание денежных сумм
        Money m1("123.45");
        Money m2("67.89");
        Money m3("-50.00");
        
        std::cout << "m1: " << m1.toString() << std::endl;
        std::cout << "m2: " << m2.toString() << std::endl;
        std::cout << "m3: " << m3.toString() << std::endl;
        
        // Арифметические операции
        Money sum = m1.add(m2);
        Money diff = m1.subtract(m2);
        Money product = m1.multiply(2);
        Money quotient = m1.divide(3);
        
        std::cout << "\nАрифметические операции:" << std::endl;
        std::cout << "m1 + m2 = " << sum.toString() << std::endl;
        std::cout << "m1 - m2 = " << diff.toString() << std::endl;
        std::cout << "m1 * 2 = " << product.toString() << std::endl;
        std::cout << "m1 / 3 = " << quotient.toString() << std::endl;
        
        // Операции сравнения
        std::cout << "\nОперации сравнения:" << std::endl;
        std::cout << "m1 == m2: " << m1.equals(m2) << std::endl;
        std::cout << "m1 < m2: " << m1.lessThan(m2) << std::endl;
        std::cout << "m1 > m2: " << m1.greaterThan(m2) << std::endl;
        
        // Работа с отрицательными числами
        Money neg_sum = m1.add(m3);
        std::cout << "\nРабота с отрицательными значениями:" << std::endl;
        std::cout << "m1 + m3 = " << neg_sum.toString() << std::endl;
        std::cout << "m3 < 0: " << m3.isNegative() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    
    return 0;
}