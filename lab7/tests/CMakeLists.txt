# Настройка Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Исполняемый файл тестов
add_executable(dungeon_tests
    test_npc.cpp
    test_factory.cpp
    test_visitor.cpp
    test_observer.cpp
    test_game.cpp
    test_battle.cpp
)

# Связываем с Google Test и основным проектом
target_link_libraries(dungeon_tests
    dungeon_lib
    gtest_main
    gmock
)

# Пути к заголовочным файлам
target_include_directories(dungeon_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Дополнительные флаги компиляции
target_compile_options(dungeon_tests PRIVATE
    -Wall -Wextra -Wpedantic -pthread
)

# Добавляем тесты в CTest
include(GoogleTest)
gtest_discover_tests(dungeon_tests)