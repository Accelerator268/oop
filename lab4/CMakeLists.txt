cmake_minimum_required(VERSION 3.16)
project(FiguresProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Основная программа
add_executable(figures_program
    main.cpp
    src/figure.cpp
)

#Тесты (если нужны)
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
    
    add_executable(figures_tests
        tests/test_figures.cpp
        src/figure.cpp
    )
    
    target_link_libraries(figures_tests GTest::gtest GTest::gtest_main)
    add_test(NAME figures_tests COMMAND figures_tests)
endif()

#Установка include директории
target_include_directories(figures_program PRIVATE include)
if(BUILD_TESTS)
    target_include_directories(figures_tests PRIVATE include)
endif()