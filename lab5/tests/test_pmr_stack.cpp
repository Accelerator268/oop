#include "../include/custom_memory_resource.h"
#include "../include/pmr_stack.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>

//Тестируемая структура с несколькими полями
struct ComplexType {
    int id;
    std::string name;
    double value;
    
    ComplexType(int i = 0, std::string n = "", double v = 0.0)
        : id(i), name(std::move(n)), value(v) {}
    
    bool operator==(const ComplexType& other) const {
        return id == other.id && name == other.name && value == other.value;
    }
};

class PMRStackTest : public ::testing::Test {
protected:
    void SetUp() override {
        resource = std::make_unique<CustomMemoryResource>();
    }
    
    void TearDown() override {
        resource.reset();
    }
    
    std::unique_ptr<CustomMemoryResource> resource;
};

//Тесты с простыми типами (int)
TEST_F(PMRStackTest, PushPopInt) {
    PMRStack<int> stack(resource.get());
    
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
    
    stack.push(10);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 10);
    
    stack.push(20);
    EXPECT_EQ(stack.size(), 2);
    EXPECT_EQ(stack.top(), 20);
    
    stack.pop();
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 10);
    
    stack.pop();
    EXPECT_TRUE(stack.empty());
}

TEST_F(PMRStackTest, EmplaceInt) {
    PMRStack<int> stack(resource.get());
    
    stack.emplace(100);
    stack.emplace(200);
    
    EXPECT_EQ(stack.top(), 200);
    stack.pop();
    EXPECT_EQ(stack.top(), 100);
}

TEST_F(PMRStackTest, CopyConstructorInt) {
    PMRStack<int> stack1(resource.get());
    stack1.push(1);
    stack1.push(2);
    stack1.push(3);
    
    PMRStack<int> stack2 = stack1;
    
    EXPECT_EQ(stack1.size(), stack2.size());
    
    //Проверяем элементы в стеке
    while (!stack1.empty() && !stack2.empty()) {
        EXPECT_EQ(stack1.top(), stack2.top());
        stack1.pop();
        stack2.pop();
    }
    EXPECT_TRUE(stack1.empty());
    EXPECT_TRUE(stack2.empty());
}

TEST_F(PMRStackTest, MoveConstructorInt) {
    PMRStack<int> stack1(resource.get());
    stack1.push(1);
    stack1.push(2);
    
    PMRStack<int> stack2 = std::move(stack1);
    
    EXPECT_TRUE(stack1.empty());
    EXPECT_EQ(stack2.size(), 2);
    EXPECT_EQ(stack2.top(), 2);
}

//Тесты с комплексными типами
TEST_F(PMRStackTest, PushPopComplexType) {
    PMRStack<ComplexType> stack(resource.get());
    
    stack.push(ComplexType{1, "first", 10.5});
    stack.emplace(2, "second", 20.5);
    stack.emplace(3, "third", 30.5);
    
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack.top().id, 3);
    EXPECT_EQ(stack.top().name, "third");
    EXPECT_EQ(stack.top().value, 30.5);
    
    stack.pop();
    EXPECT_EQ(stack.top().id, 2);
    EXPECT_EQ(stack.top().name, "second");
}

TEST_F(PMRStackTest, IteratorsComplexType) {
    PMRStack<ComplexType> stack(resource.get());
    
    stack.emplace(1, "one", 1.1);
    stack.emplace(2, "two", 2.2);
    stack.emplace(3, "three", 3.3);
    
    //Проверка итератора
    int count = 0;
    for (auto it = stack.begin(); it != stack.end(); ++it) {
        ++count;
        (void)*it; //Используем переменную, чтобы избежать предупреждения
    }
    EXPECT_EQ(count, 3);
    
    //Проверка range-based for
    count = 0;
    for (const auto& item : stack) {
        ++count;
        (void)item; //Используем переменную, чтобы избежать предупреждения
    }
    EXPECT_EQ(count, 3);
}

TEST_F(PMRStackTest, ClearComplexType) {
    PMRStack<ComplexType> stack(resource.get());
    
    for (int i = 0; i < 10; ++i) {
        stack.emplace(i, "item" + std::to_string(i), i * 1.1);
    }
    
    EXPECT_EQ(stack.size(), 10);
    stack.clear();
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

//Тесты аллокатора и переиспользования памяти
TEST_F(PMRStackTest, MemoryReuse) {
    {
        PMRStack<int> stack(resource.get());
        
        //Выделяем память
        for (int i = 0; i < 5; ++i) {
            stack.push(i);
        }
        
        size_t active_before = resource->active_blocks_count();
        
        //Освобождаем память
        while (!stack.empty()) {
            stack.pop();
        }
        
        //Снова выделяем - должна переиспользоваться память
        for (int i = 0; i < 5; ++i) {
            stack.push(i * 10);
        }
        
        //Проверяем, что память переиспользуется
        EXPECT_LE(resource->active_blocks_count(), active_before + 1);
    }
    
    //После выхода из области видимости вся память должна быть освобождена
    EXPECT_EQ(resource->active_blocks_count(), 0);
}

TEST_F(PMRStackTest, InitializerList) {
    PMRStack<int> stack({1, 2, 3, 4, 5}, resource.get());
    
    EXPECT_EQ(stack.size(), 5);
    
    //В стеке элементы должны быть в порядке 5, 4, 3, 2, 1
    EXPECT_EQ(stack.top(), 1);
    stack.pop();
    EXPECT_EQ(stack.top(), 2);
}

TEST_F(PMRStackTest, AssignmentOperators) {
    PMRStack<int> stack1(resource.get());
    stack1.push(100);
    stack1.push(200);
    
    //Копирующее присваивание
    PMRStack<int> stack2(resource.get());
    stack2 = stack1;
    
    EXPECT_EQ(stack1.size(), stack2.size());
    
    //Восстанавливаем стеки для проверки содержимого
    PMRStack<int> temp1 = stack1;
    PMRStack<int> temp2 = stack2;
    
    while (!temp1.empty() && !temp2.empty()) {
        EXPECT_EQ(temp1.top(), temp2.top());
        temp1.pop();
        temp2.pop();
    }
    
    //Перемещающее присваивание
    PMRStack<int> stack3(resource.get());
    stack3 = std::move(stack1);
    
    EXPECT_TRUE(stack1.empty());
    EXPECT_EQ(stack3.size(), 2);
    EXPECT_EQ(stack3.top(), 200);
}

TEST_F(PMRStackTest, ExceptionSafety) {
    PMRStack<ComplexType> stack(resource.get());
    
    //Добавляем несколько элементов
    for (int i = 0; i < 3; ++i) {
        stack.emplace(i, "test", i * 1.0);
    }
    
    EXPECT_EQ(stack.size(), 3);
    
    //Попытка доступа к top пустого стека после очистки
    stack.clear();
    EXPECT_THROW(stack.top(), std::out_of_range);
}

TEST_F(PMRStackTest, GetMemoryResource) {
    PMRStack<int> stack(resource.get());
    
    auto mr = stack.get_memory_resource();
    EXPECT_EQ(mr, resource.get());
    
    stack.push(42);
    stack.push(43);
    
    //Проверяем, что стек работает корректно
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.top(), 43);
}

TEST_F(PMRStackTest, MemoryLeakCheck) {
    {
        PMRStack<int> stack(resource.get());
        
        for (int i = 0; i < 1000; ++i) {
            stack.push(i);
        }
        
        //Автоматическое освобождение при выходе из области видимости
    }
    
    //После выхода из области видимости вся память должна быть освобождена
    EXPECT_EQ(resource->active_blocks_count(), 0);
    EXPECT_EQ(resource->active_bytes(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}