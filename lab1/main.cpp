#include <iostream>
#include "include/convert.h"

int main() {
    int hour, minute;
    std::string period;
    char colon;
    
    std::cout << "Введите время в 12-часовом формате: ";
    
    // Чтение входных данных
    std::cin >> hour >> colon >> minute >> period;
    
    // Проверка корректности ввода
    if (hour < 1 || hour > 12 || minute < 0 || minute > 59 || (period != "am" && period != "pm")) {
        std::cout << "Ошибка: некорректный формат времени" << std::endl;
        return 1;
    }
    
    // Преобразование времени
    std::string result = convertTo24Hour(hour, minute, period);
    
    // Вывод результата
    std::cout << "Время в 24-часовом формате: " << result << std::endl;
    
    return 0;
}