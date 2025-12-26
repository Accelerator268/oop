cmake_minimum_required(VERSION 3.10)
project(dungeon_simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка компилятора
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -pthread)
endif()

# Основная библиотека
add_library(dungeon_lib
    src/npc.cpp
    src/dragon.cpp
    src/knight.cpp
    src/pegasus.cpp
    src/factory.cpp
    src/visitor.cpp
    src/observer.cpp
    src/game.cpp
)

# Основное приложение
add_executable(dungeon_simulator
    main.cpp
)

target_link_libraries(dungeon_simulator
    dungeon_lib
    pthread
)

# Пути к заголовочным файлам
target_include_directories(dungeon_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(dungeon_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Поддиректория с тестами
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    add_subdirectory(tests)
endif()