#include "include/custom_memory_resource.h"
#include "include/pmr_stack.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Демонстрационная структура
struct Employee {
    int id;
    std::string name;
    double salary;
    
    Employee(int i = 0, std::string n = "", double s = 0.0)
        : id(i), name(std::move(n)), salary(s) {}
    
    void print() const {
        std::cout << "Employee [ID: " << id 
                  << ", Name: " << name 
                  << ", Salary: " << salary << "]\n";
    }
};

void demo_simple_types() {
    std::cout << "Демонстрация работы с простыми типами (int)\n";
    
    auto resource = std::make_unique<CustomMemoryResource>();
    
    {
        PMRStack<int> int_stack(resource.get());
        
        std::cout << "\nДобавляем элементы в стек:\n";
        for (int i = 1; i <= 5; ++i) {
            int_stack.push(i * 10);
            std::cout << "Добавлен: " << i * 10 << std::endl;
        }
        
        std::cout << "\nРазмер стека: " << int_stack.size() << std::endl;
        std::cout << "Верхний элемент: " << int_stack.top() << std::endl;
        
        std::cout << "\nИтерирование по стеку:\n";
        for (const auto& item : int_stack) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
        
        std::cout << "\nИзвлекаем элементы:\n";
        while (!int_stack.empty()) {
            std::cout << "Извлечен: " << int_stack.top() << std::endl;
            int_stack.pop();
        }
        
        std::cout << "\nСтатистика аллокатора:\n";
        std::cout << "Всего блоков: " << resource->allocated_blocks_count() << std::endl;
        std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
        std::cout << "Всего байт: " << resource->total_allocated_bytes() << std::endl;
        std::cout << "Активных байт: " << resource->active_bytes() << std::endl;
    }
    
    std::cout << "\nПосле выхода из области видимости:\n";
    std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
}

void demo_complex_types() {
    std::cout << "\n\nДемонстрация работы со сложными типами (Employee)\n";
    
    auto resource = std::make_unique<CustomMemoryResource>();
    
    {
        PMRStack<Employee> emp_stack(resource.get());
        
        std::cout << "\nДобавляем сотрудников:\n";
        emp_stack.emplace(1, "Иван Иванов", 50000.0);
        emp_stack.emplace(2, "Петр Петров", 60000.0);
        emp_stack.emplace(3, "Мария Сидорова", 55000.0);
        
        std::cout << "\nИнформация о верхнем сотруднике:\n";
        emp_stack.top().print();
        
        std::cout << "\nВсе сотрудники в стеке (сверху вниз):\n";
        for (const auto& emp : emp_stack) {
            emp.print();
        }
        
        std::cout << "\nКлонируем стек:\n";
        auto emp_stack_copy = emp_stack;
        
        std::cout << "Оригинальный стек (" << emp_stack.size() << " элементов):\n";
        for (const auto& emp : emp_stack) {
            emp.print();
        }
        
        std::cout << "\nКопия стека (" << emp_stack_copy.size() << " элементов):\n";
        for (const auto& emp : emp_stack_copy) {
            emp.print();
        }
        
        std::cout << "\nСтатистика аллокатора:\n";
        std::cout << "Всего блоков: " << resource->allocated_blocks_count() << std::endl;
        std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
    }
    
    std::cout << "\nПосле выхода из области видимости:\n";
    std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
}

void demo_memory_reuse() {
    std::cout << "\n\nДемонстрация переиспользования памяти\n";
    
    auto resource = std::make_unique<CustomMemoryResource>();
    
    std::cout << "\nЦикл 1: Создаем и заполняем стек\n";
    {
        PMRStack<std::string> str_stack(resource.get());
        
        for (int i = 0; i < 3; ++i) {
            str_stack.push("Строка_" + std::to_string(i));
        }
        
        std::cout << "Активных блоков после заполнения: " 
                  << resource->active_blocks_count() << std::endl;
    }
    
    std::cout << "\nПосле уничтожения стека:\n";
    std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
    std::cout << "Всего блоков в пуле: " << resource->allocated_blocks_count() << std::endl;
    
    std::cout << "\nЦикл 2: Создаем новый стек\n";
    {
        PMRStack<std::string> str_stack(resource.get());
        
        for (int i = 10; i < 13; ++i) {
            str_stack.push("Новая_строка_" + std::to_string(i));
        }
        
        std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
        std::cout << "Всего блоков в пуле: " << resource->allocated_blocks_count() << std::endl;
        std::cout << "Память переиспользуется из пула!\n";
    }
    
    std::cout << "\nСтатистика после всех операций:\n";
    std::cout << "Всего выделено блоков: " << resource->allocated_blocks_count() << std::endl;
    std::cout << "Активных блоков: " << resource->active_blocks_count() << std::endl;
}

int main() {
    std::cout << "Демонстрация PMR Stack с пользовательским аллокатором\n";
    
    try {
        demo_simple_types();
        demo_complex_types();
        demo_memory_reuse();
        
        std::cout << "\n\nВсе демонстрации завершены успешно!\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}