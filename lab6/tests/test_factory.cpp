#include <gtest/gtest.h>
#include <sstream>
#include "factory.h"
#include "dragon.h"
#include "knight.h"
#include "pegasus.h"

class FactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        //Создаем несколько NPC для тестирования
        dragon = std::dynamic_pointer_cast<Dragon>(
            NPCFactory::createNPC(NpcType::Dragon, 10, 20, "TestDragon"));
        knight = std::dynamic_pointer_cast<Knight>(
            NPCFactory::createNPC(NpcType::Knight, 30, 40, "TestKnight"));
        pegasus = std::dynamic_pointer_cast<Pegasus>(
            NPCFactory::createNPC(NpcType::Pegasus, 50, 60, "TestPegasus"));
    }
    
    std::shared_ptr<Dragon> dragon;
    std::shared_ptr<Knight> knight;
    std::shared_ptr<Pegasus> pegasus;
};

TEST_F(FactoryTest, CreateNPC) {
    EXPECT_NE(dragon, nullptr);
    EXPECT_EQ(dragon->getType(), NpcType::Dragon);
    EXPECT_EQ(dragon->getName(), "TestDragon");
    
    EXPECT_NE(knight, nullptr);
    EXPECT_EQ(knight->getType(), NpcType::Knight);
    EXPECT_EQ(knight->getName(), "TestKnight");
    
    EXPECT_NE(pegasus, nullptr);
    EXPECT_EQ(pegasus->getType(), NpcType::Pegasus);
    EXPECT_EQ(pegasus->getName(), "TestPegasus");
}

TEST_F(FactoryTest, CreateDefaultNames) {
    auto defaultDragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "");
    EXPECT_EQ(defaultDragon->getName(), "Dragon");
    
    auto defaultKnight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "");
    EXPECT_EQ(defaultKnight->getName(), "Knight");
    
    auto defaultPegasus = NPCFactory::createNPC(NpcType::Pegasus, 0, 0, "");
    EXPECT_EQ(defaultPegasus->getName(), "Pegasus");
}

TEST_F(FactoryTest, LoadNPC) {
    //Создаем поток с данными NPC
    std::stringstream ss;
    ss << static_cast<int>(NpcType::Dragon) << std::endl;
    ss << 100 << std::endl;
    ss << 200 << std::endl;
    ss << "LoadedDragon" << std::endl;
    
    auto loaded = NPCFactory::loadNPC(ss);
    EXPECT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->getType(), NpcType::Dragon);
    EXPECT_EQ(loaded->getX(), 100);
    EXPECT_EQ(loaded->getY(), 200);
    EXPECT_EQ(loaded->getName(), "LoadedDragon");
}

TEST_F(FactoryTest, LoadInvalidNPC) {
    //Тест с неверными данными
    std::stringstream ss;
    ss << "invalid" << std::endl; //Не число
    
    auto loaded = NPCFactory::loadNPC(ss);
    EXPECT_EQ(loaded, nullptr);
}

TEST_F(FactoryTest, LoadInvalidType) {
    //Тест с несуществующим типом
    std::stringstream ss;
    ss << 99 << std::endl; //Несуществующий тип
    
    auto loaded = NPCFactory::loadNPC(ss);
    EXPECT_EQ(loaded, nullptr);
}

TEST_F(FactoryTest, TypeConversion) {
    EXPECT_EQ(NPCFactory::getTypeFromString("Dragon"), NpcType::Dragon);
    EXPECT_EQ(NPCFactory::getTypeFromString("dragon"), NpcType::Dragon);
    EXPECT_EQ(NPCFactory::getTypeFromString("1"), NpcType::Dragon);
    
    EXPECT_EQ(NPCFactory::getTypeFromString("Knight"), NpcType::Knight);
    EXPECT_EQ(NPCFactory::getTypeFromString("knight"), NpcType::Knight);
    EXPECT_EQ(NPCFactory::getTypeFromString("2"), NpcType::Knight);
    
    EXPECT_EQ(NPCFactory::getTypeFromString("Pegasus"), NpcType::Pegasus);
    EXPECT_EQ(NPCFactory::getTypeFromString("pegasus"), NpcType::Pegasus);
    EXPECT_EQ(NPCFactory::getTypeFromString("3"), NpcType::Pegasus);
    
    EXPECT_EQ(NPCFactory::getTypeFromString("Invalid"), NpcType::Unknown);
}

TEST_F(FactoryTest, StringConversion) {
    EXPECT_EQ(NPCFactory::getStringFromType(NpcType::Dragon), "Dragon");
    EXPECT_EQ(NPCFactory::getStringFromType(NpcType::Knight), "Knight");
    EXPECT_EQ(NPCFactory::getStringFromType(NpcType::Pegasus), "Pegasus");
    EXPECT_EQ(NPCFactory::getStringFromType(NpcType::Unknown), "Unknown");
}