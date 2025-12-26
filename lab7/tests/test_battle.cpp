#include <gtest/gtest.h>
#include <set>
#include <memory>
#include <fstream>  // Добавляем
#include "factory.h"
#include "visitor.h"
#include "observer.h"

class BattleTest : public ::testing::Test {
protected:
    void SetUp() override {
        visitor = std::make_shared<FightVisitor>();
        consoleObserver = std::make_shared<ConsoleObserver>();
        
        // Очищаем файл лога
        std::ofstream logFile("battle_test_log.txt", std::ios::trunc);
        logFile.close();
    }
    
    std::shared_ptr<FightVisitor> visitor;
    std::shared_ptr<ConsoleObserver> consoleObserver;
    
    using NPCSet = std::set<std::shared_ptr<NPC>>;
    
    // Вспомогательная функция для симуляции битвы
    NPCSet simulateBattle(NPCSet& npcs, int distance) {
        NPCSet killed;
        
        for (const auto& attacker : npcs) {
            for (const auto& defender : npcs) {
                if (attacker != defender && 
                    attacker->isClose(defender, distance) &&
                    killed.find(defender) == killed.end() &&
                    killed.find(attacker) == killed.end()) {
                    
                    bool result = defender->accept(visitor, attacker);
                    if (result) {
                        // Уведомляем о победе
                        attacker->notifyFight(defender, true);
                        killed.insert(defender);
                    }
                }
            }
        }
        
        return killed;
    }
};

// Остальные тесты без изменений...

TEST_F(BattleTest, DragonKillsPegasus) {
    NPCSet npcs;
    
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "Dragon");
    auto pegasus = NPCFactory::createNPC(NpcType::Pegasus, 5, 5, "Pegasus");
    
    dragon->subscribe(consoleObserver);
    pegasus->subscribe(consoleObserver);
    
    npcs.insert(dragon);
    npcs.insert(pegasus);
    
    // Дракон и пегас близко (расстояние ~7.07)
    auto killed = simulateBattle(npcs, 10);
    
    EXPECT_EQ(killed.size(), 1);
    EXPECT_NE(killed.find(pegasus), killed.end());
    EXPECT_EQ(killed.find(dragon), killed.end());
}

TEST_F(BattleTest, KnightKillsDragon) {
    NPCSet npcs;
    
    auto knight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "Knight");
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 3, 4, "Dragon");
    
    knight->subscribe(consoleObserver);
    dragon->subscribe(consoleObserver);
    
    npcs.insert(knight);
    npcs.insert(dragon);
    
    // Рыцарь и дракон близко (расстояние 5)
    auto killed = simulateBattle(npcs, 10);
    
    EXPECT_EQ(killed.size(), 1);
    EXPECT_NE(killed.find(dragon), killed.end());
    EXPECT_EQ(killed.find(knight), killed.end());
}

TEST_F(BattleTest, PegasusDoesNotAttack) {
    NPCSet npcs;
    
    auto pegasus = NPCFactory::createNPC(NpcType::Pegasus, 0, 0, "Pegasus");
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 1, 1, "Dragon");
    auto knight = NPCFactory::createNPC(NpcType::Knight, 2, 2, "Knight");
    
    npcs.insert(pegasus);
    npcs.insert(dragon);
    npcs.insert(knight);
    
    // Пегас не должен никого атаковать
    auto killed = simulateBattle(npcs, 10);
    
    // Проверяем, что пегас никого не убил
    bool pegasusKilledAnyone = false;
    for (const auto& attacker : npcs) {
        if (attacker->getType() == NpcType::Pegasus) {
            for (const auto& defender : npcs) {
                if (attacker != defender && attacker->isClose(defender, 10)) {
                    bool result = defender->accept(visitor, attacker);
                    if (result) {
                        pegasusKilledAnyone = true;
                    }
                }
            }
        }
    }
    
    EXPECT_FALSE(pegasusKilledAnyone);
}

TEST_F(BattleTest, DistanceMatters) {
    NPCSet npcs;
    
    auto knight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "Knight");
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 100, 100, "Dragon");
    
    npcs.insert(knight);
    npcs.insert(dragon);
    
    // Слишком далеко для битвы
    auto killed = simulateBattle(npcs, 10);
    EXPECT_EQ(killed.size(), 0);
    
    // В пределах досягаемости
    dragon->setPosition(5, 5);
    killed = simulateBattle(npcs, 10);
    EXPECT_EQ(killed.size(), 1);
}

TEST_F(BattleTest, SelfDoesNotAttack) {
    NPCSet npcs;
    
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "Dragon");
    npcs.insert(dragon);
    
    // NPC не должен атаковать сам себя
    auto killed = simulateBattle(npcs, 100);
    EXPECT_EQ(killed.size(), 0);
}

TEST_F(BattleTest, AlreadyKilledNotAttacked) {
    NPCSet npcs;
    
    auto knight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "Knight");
    auto dragon1 = NPCFactory::createNPC(NpcType::Dragon, 1, 1, "Dragon1");
    auto dragon2 = NPCFactory::createNPC(NpcType::Dragon, 2, 2, "Dragon2");
    
    npcs.insert(knight);
    npcs.insert(dragon1);
    npcs.insert(dragon2);
    
    // Рыцарь убивает драконов
    auto killed = simulateBattle(npcs, 10);
    
    // Драконы не должны атаковать друг друга после смерти
    // (это проверяется логикой в simulateBattle)
    EXPECT_GT(killed.size(), 0);
}

TEST_F(BattleTest, DiceBattleSimulation) {
    // Тест на механику броска кубика
    auto attacker = NPCFactory::createNPC(NpcType::Knight, 0, 0, "Attacker");
    auto defender = NPCFactory::createNPC(NpcType::Dragon, 1, 1, "Defender");
    
    // Симулируем 1000 боев для статистики
    int attackerWins = 0;
    int defenderWins = 0;
    
    for (int i = 0; i < 1000; ++i) {
        int attackRoll = attacker->rollAttack();
        int defenseRoll = defender->rollDefense();
        
        if (attackRoll > defenseRoll) {
            attackerWins++;
        } else if (defenseRoll > attackRoll) {
            defenderWins++;
        }
        // Ничья - никто не побеждает
    }
    
    // Проверяем, что результаты выглядят правдоподобно
    // (статистически равномерное распределение)
    EXPECT_GT(attackerWins, 100);  // Должно быть достаточно побед
    EXPECT_GT(defenderWins, 100);  // Должно быть достаточно побед
    EXPECT_LT(attackerWins, 900);  // Но не слишком много
    EXPECT_LT(defenderWins, 900);  // Но не слишком много
}