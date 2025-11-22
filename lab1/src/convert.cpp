#include "../include/convert.h"

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
    } else {
        if (hour == 12) {
            // 12:00 pm остается 12:00
            hour24 = 12;
        } else {
            // Все остальные PM часы увеличиваются на 12
            hour24 = hour + 12;
        }
    } //8:03 = 0803
    
    // Форматирование результата в виде четырехсимвольной строки
    std::ostringstream result;
    result << std::setw(2) << std::setfill('0') << hour24
           << std::setw(2) << std::setfill('0') << minute;
    
    return result.str();
}