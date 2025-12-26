cmake_minimum_required(VERSION 3.10)
project(memory_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Настройка компилятора
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -pthread)
endif()

#Установка Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#Основная библиотека
add_library(memory_project_lib
    src/custom_memory_resource.cpp
)

#Основное приложение
add_executable(memory_project_exe
    main.cpp
)

target_link_libraries(memory_project_exe
    memory_project_lib
)

#Для Windows нужна дополнительная библиотека для _aligned_malloc
if(MSVC)
    target_link_libraries(memory_project_lib ${CMAKE_DL_LIBS})
endif()

#Тесты
add_executable(tests
    tests/test_pmr_stack.cpp
)

target_link_libraries(tests
    memory_project_lib
    gtest_main
)

#Добавление тестов в CTest
include(CTest)
enable_testing()
add_test(NAME PMRStackTests COMMAND tests)

#Установка путей для заголовочных файлов
target_include_directories(memory_project_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(memory_project_exe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)