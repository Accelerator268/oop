#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

/**
 * Функция для преобразования 12-часового времени в 24-часовой формат
 * @param hour - час (1-12)
 * @param minute - минута (0-59)
 * @param period - период ("am" или "pm")
 * @return строка в формате "HHMM" (четыре символа)
 */

std::string convertTo24Hour(int hour, int minute, const std::string& period) {
    int hour24;
    
    // Преобразование часов согласно правилам 12-часового формата
    if (period == "am") {
        if (hour == 12) {
            // 12:00 am становится 00:00
            hour24 = 0;
        } else {
            // Все остальные AM часы остаются такими же
            hour24 = hour;
        }
    } else { // period == "pm"
        if (hour == 12) {
            // 12:00 pm остается 12:00
            hour24 = 12;
        } else {
            // Все остальные PM часы увеличиваются на 12
            hour24 = hour + 12;
        }
    }
    
    // Форматирование результата в виде четырехсимвольной строки
    std::ostringstream result;
    result << std::setw(2) << std::setfill('0') << hour24
           << std::setw(2) << std::setfill('0') << minute;
    
    return result.str();
}

int main() {
    int hour, minute;
    std::string period;
    char colon;
    
    std::cout << "Введите время в 12-часовом формате (например: 8:30 am): ";
    
    // Чтение входных данных
    std::cin >> hour >> colon >> minute >> period;
    
    // Проверка корректности ввода (базовая валидация)
    if (hour < 1 || hour > 12 || minute < 0 || minute > 59 || 
        (period != "am" && period != "pm")) {
        std::cout << "Ошибка: некорректный формат времени!" << std::endl;
        return 1;
    }
    
    // Преобразование времени
    std::string result = convertTo24Hour(hour, minute, period);
    
    // Вывод результата
    std::cout << "Время в 24-часовом формате: " << result << std::endl;
    
    return 0;
}