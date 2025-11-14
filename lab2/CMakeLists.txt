cmake_minimum_required(VERSION 3.10)
project(MoneyContainer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создание исполняемого файла
add_executable(money_app
    money.cpp
    main.cpp
)

# Настройка компилятора
if(MSVC)
    target_compile_options(money_app PRIVATE /W4)
else()
    target_compile_options(money_app PRIVATE -Wall -Wextra -pedantic)
endif()

# Установка выходной директории
set_target_properties(money_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)