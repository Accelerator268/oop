#include <gtest/gtest.h>
#include <memory>
#include "visitor.h"
#include "dragon.h"
#include "knight.h"
#include "pegasus.h"

class VisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        visitor = std::make_shared<FightVisitor>();
        
        dragon = std::make_shared<Dragon>(0, 0, "TestDragon");
        knight = std::make_shared<Knight>(0, 0, "TestKnight");
        pegasus = std::make_shared<Pegasus>(0, 0, "TestPegasus");
    }
    
    std::shared_ptr<FightVisitor> visitor;
    std::shared_ptr<Dragon> dragon;
    std::shared_ptr<Knight> knight;
    std::shared_ptr<Pegasus> pegasus;
};

TEST_F(VisitorTest, DragonVsKnight) {
    // Рыцарь атакует дракона
    bool result = visitor->visit(
        std::dynamic_pointer_cast<Dragon>(dragon),
        knight
    );
    EXPECT_TRUE(result); // Рыцарь может убить дракона
    
    // Дракон атакует рыцаря
    result = visitor->visit(
        std::dynamic_pointer_cast<Knight>(knight),
        dragon
    );
    EXPECT_FALSE(result); // Дракон не может убить рыцаря
}

TEST_F(VisitorTest, DragonVsPegasus) {
    // Дракон атакует пегаса
    bool result = visitor->visit(
        std::dynamic_pointer_cast<Pegasus>(pegasus),
        dragon
    );
    EXPECT_TRUE(result); // Дракон может есть пегаса
    
    // Пегас атакует дракона
    result = visitor->visit(
        std::dynamic_pointer_cast<Dragon>(dragon),
        pegasus
    );
    EXPECT_FALSE(result); // Пегас не может атаковать дракона
}

TEST_F(VisitorTest, KnightVsPegasus) {
    // Рыцарь атакует пегаса
    bool result = visitor->visit(
        std::dynamic_pointer_cast<Pegasus>(pegasus),
        knight
    );
    EXPECT_FALSE(result); // Рыцарь не может атаковать пегаса
    
    // Пегас атакует рыцаря
    result = visitor->visit(
        std::dynamic_pointer_cast<Knight>(knight),
        pegasus
    );
    EXPECT_FALSE(result); // Пегас не может атаковать рыцаря
}

TEST_F(VisitorTest, SameTypeFights) {
    // Дракон vs Дракон
    auto anotherDragon = std::make_shared<Dragon>(0, 0, "AnotherDragon");
    bool result = visitor->visit(
        std::dynamic_pointer_cast<Dragon>(dragon),
        anotherDragon
    );
    EXPECT_FALSE(result); // Драконы не могут атаковать друг друга
    
    // Рыцарь vs Рыцарь
    auto anotherKnight = std::make_shared<Knight>(0, 0, "AnotherKnight");
    result = visitor->visit(
        std::dynamic_pointer_cast<Knight>(knight),
        anotherKnight
    );
    EXPECT_FALSE(result); // Рыцари не могут атаковать друг друга
    
    // Пегас vs Пегас
    auto anotherPegasus = std::make_shared<Pegasus>(0, 0, "AnotherPegasus");
    result = visitor->visit(
        std::dynamic_pointer_cast<Pegasus>(pegasus),
        anotherPegasus
    );
    EXPECT_FALSE(result); // Пегасы не могут атаковать друг друга
}

TEST_F(VisitorTest, AcceptMethod) {
    // Тестируем метод accept у NPC
    auto attackingKnight = std::make_shared<Knight>(0, 0, "AttackingKnight");
    
    // Рыцарь атакует дракона через accept
    bool result = dragon->accept(visitor, attackingKnight);
    EXPECT_TRUE(result); // Рыцарь может убить дракона
    
    // Дракон атакует пегаса
    result = pegasus->accept(visitor, dragon);
    EXPECT_TRUE(result); // Дракон может съесть пегаса
    
    // Пегас атакует рыцаря
    result = knight->accept(visitor, pegasus);
    EXPECT_FALSE(result); // Пегас не может атаковать рыцаря
}