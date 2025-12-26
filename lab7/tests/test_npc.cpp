#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include "npc.h"
#include "dragon.h"
#include "knight.h"
#include "pegasus.h"

class NPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        dragon = std::make_shared<Dragon>(100, 200, "Smaug");
        knight = std::make_shared<Knight>(150, 250, "Arthur");
        pegasus = std::make_shared<Pegasus>(200, 300, "Peggy");
    }
    
    std::shared_ptr<Dragon> dragon;
    std::shared_ptr<Knight> knight;
    std::shared_ptr<Pegasus> pegasus;
};

// Базовые тесты NPC
TEST_F(NPCTest, ConstructorAndGetters) {
    EXPECT_EQ(dragon->getType(), NpcType::Dragon);
    EXPECT_EQ(dragon->getX(), 100);
    EXPECT_EQ(dragon->getY(), 200);
    EXPECT_EQ(dragon->getName(), "Smaug");
    EXPECT_TRUE(dragon->isAlive());
    
    EXPECT_EQ(knight->getType(), NpcType::Knight);
    EXPECT_EQ(knight->getName(), "Arthur");
    EXPECT_TRUE(knight->isAlive());
    
    EXPECT_EQ(pegasus->getType(), NpcType::Pegasus);
    EXPECT_EQ(pegasus->getName(), "Peggy");
    EXPECT_TRUE(pegasus->isAlive());
}

TEST_F(NPCTest, SetPosition) {
    dragon->setPosition(50, 60);
    EXPECT_EQ(dragon->getX(), 50);
    EXPECT_EQ(dragon->getY(), 60);
    
    // Проверка границ
    dragon->setPosition(-10, 600); // Вне границ
    EXPECT_EQ(dragon->getX(), 50); // Не изменилось
    EXPECT_EQ(dragon->getY(), 60);
    
    dragon->setPosition(250, 400); // В границах
    EXPECT_EQ(dragon->getX(), 250);
    EXPECT_EQ(dragon->getY(), 400);
}

TEST_F(NPCTest, SetAlive) {
    dragon->setAlive(false);
    EXPECT_FALSE(dragon->isAlive());
    
    dragon->setAlive(true);
    EXPECT_TRUE(dragon->isAlive());
}

TEST_F(NPCTest, MoveDistance) {
    EXPECT_EQ(dragon->getMoveDistance(), 50);  // Дракон
    EXPECT_EQ(knight->getMoveDistance(), 30);  // Рыцарь
    EXPECT_EQ(pegasus->getMoveDistance(), 30); // Пегас
}

TEST_F(NPCTest, KillDistance) {
    EXPECT_EQ(dragon->getKillDistance(), 30);  // Дракон
    EXPECT_EQ(knight->getKillDistance(), 10);  // Рыцарь
    EXPECT_EQ(pegasus->getKillDistance(), 10); // Пегас
}

TEST_F(NPCTest, IsClose) {
    auto closeNPC = std::make_shared<Dragon>(105, 205, "CloseDragon");
    auto farNPC = std::make_shared<Dragon>(400, 400, "FarDragon");
    
    EXPECT_TRUE(dragon->isClose(closeNPC, 10)); // Расстояние ~7.07 < 10
    EXPECT_FALSE(dragon->isClose(farNPC, 50));  // Расстояние ~447 > 50
    EXPECT_TRUE(dragon->isClose(dragon, 0));    // Расстояние 0
}

TEST_F(NPCTest, SetName) {
    dragon->setName("NewSmaug");
    EXPECT_EQ(dragon->getName(), "NewSmaug");
    
    knight->setName("SirArthur");
    EXPECT_EQ(knight->getName(), "SirArthur");
}

TEST_F(NPCTest, RollDice) {
    // Бросок кубика должен быть в диапазоне 1-6
    for (int i = 0; i < 100; ++i) {
        int attack = dragon->rollAttack();
        int defense = dragon->rollDefense();
        
        EXPECT_GE(attack, 1);
        EXPECT_LE(attack, 6);
        EXPECT_GE(defense, 1);
        EXPECT_LE(defense, 6);
    }
}

TEST_F(NPCTest, Print) {
    std::stringstream ss;
    dragon->print(ss);
    EXPECT_NE(ss.str().find("Smaug"), std::string::npos);
    EXPECT_NE(ss.str().find("100"), std::string::npos);
    EXPECT_NE(ss.str().find("200"), std::string::npos);
}

TEST_F(NPCTest, StreamOutput) {
    std::stringstream ss;
    ss << *dragon;
    EXPECT_NE(ss.str().find("Dragon"), std::string::npos);
    EXPECT_NE(ss.str().find("Smaug"), std::string::npos);
}
