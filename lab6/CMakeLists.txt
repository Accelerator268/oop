cmake_minimum_required(VERSION 3.10)
project(dungeon_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Настройка компилятора
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#Основная библиотека
add_library(dungeon_lib
    src/npc.cpp
    src/dragon.cpp
    src/knight.cpp
    src/pegasus.cpp
    src/factory.cpp
    src/visitor.cpp
    src/observer.cpp
)

#Основное приложение
add_executable(dungeon_editor
    main.cpp
)

target_link_libraries(dungeon_editor
    dungeon_lib
)

#Пути к заголовочным файлам
target_include_directories(dungeon_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(dungeon_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#Поддиректория с тестами
add_subdirectory(tests)