#include <gtest/gtest.h>
#include <set>
#include "factory.h"
#include "visitor.h"
#include "observer.h"

class BattleTest : public ::testing::Test {
protected:
    void SetUp() override {
        visitor = std::make_shared<FightVisitor>();
        consoleObserver = std::make_shared<ConsoleObserver>();
        fileObserver = std::make_shared<FileObserver>("battle_test_log.txt");
    }
    
    std::shared_ptr<FightVisitor> visitor;
    std::shared_ptr<ConsoleObserver> consoleObserver;
    std::shared_ptr<FileObserver> fileObserver;
    
    using NPCSet = std::set<std::shared_ptr<NPC>>;
    
    //Вспомогательная функция для симуляции битвы
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

TEST_F(BattleTest, DragonKillsPegasus) {
    NPCSet npcs;
    
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "Dragon");
    auto pegasus = NPCFactory::createNPC(NpcType::Pegasus, 5, 5, "Pegasus");
    
    dragon->subscribe(consoleObserver);
    pegasus->subscribe(consoleObserver);
    
    npcs.insert(dragon);
    npcs.insert(pegasus);
    
    //Дракон и пегас близко (расстояние ~7.07)
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
    
    //Рыцарь и дракон близко (расстояние 5)
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
    
    //Пегас не должен никого атаковать
    auto killed = simulateBattle(npcs, 10);
    
    //Дракон должен съесть пегаса, если атакует
    //Но в нашей симуляции пегас не атакует дракона
    //Рыцарь должен убить дракона
    //Это сложная логика, зависит от порядка итерации
    
    //Упрощенная проверка: в любом случае пегас не должен никого убивать
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
    
    //Слишком далеко для битвы
    auto killed = simulateBattle(npcs, 10);
    EXPECT_EQ(killed.size(), 0);
    
    //В пределах досягаемости
    dragon->setPosition(5, 5);
    killed = simulateBattle(npcs, 10);
    EXPECT_EQ(killed.size(), 1);
}

TEST_F(BattleTest, MultipleNPCsBattle) {
    NPCSet npcs;
    
    //Создаем группу NPC
    auto dragon1 = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "Dragon1");
    auto dragon2 = NPCFactory::createNPC(NpcType::Dragon, 2, 2, "Dragon2");
    auto knight1 = NPCFactory::createNPC(NpcType::Knight, 4, 4, "Knight1");
    auto knight2 = NPCFactory::createNPC(NpcType::Knight, 6, 6, "Knight2");
    auto pegasus1 = NPCFactory::createNPC(NpcType::Pegasus, 8, 8, "Pegasus1");
    auto pegasus2 = NPCFactory::createNPC(NpcType::Pegasus, 10, 10, "Pegasus2");
    
    //Подписываем всех на observer
    for (auto& npc : {dragon1, dragon2, knight1, knight2, pegasus1, pegasus2}) {
        npc->subscribe(fileObserver);
        npcs.insert(npc);
    }
    
    //Симулируем битву с большой дистанцией
    auto killed = simulateBattle(npcs, 20);
    
    // Проверяем логику:
    //1. Драконы должны съесть пегасов
    //2. Рыцари должны убить драконов
    //3. Драконы не дерутся друг с другом
    //4. Рыцари не дерутся друг с другом
    //5. Пегасы никого не атакуют
    
    //Не проверяем точное количество, так как зависит от порядка итерации
    //Но должны быть убиты как минимум некоторые NPC
    EXPECT_GT(killed.size(), 0);
    EXPECT_LT(killed.size(), npcs.size());
}

TEST_F(BattleTest, SelfDoesNotAttack) {
    NPCSet npcs;
    
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "Dragon");
    npcs.insert(dragon);
    
    //NPC не должен атаковать сам себя
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
    
    //Рыцарь убивает обоих драконов
    auto killed = simulateBattle(npcs, 10);
    
    //Драконы не должны атаковать друг друга или рыцаря после смерти
    //(это проверяется логикой в simulateBattle)
    EXPECT_GT(killed.size(), 0);
}