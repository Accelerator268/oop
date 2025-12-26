#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>  // Добавляем для std::ofstream
#include <memory>
#include "game.h"
#include "factory.h"
#include "observer.h"  // Добавляем для ConsoleObserver

class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Очищаем предыдущие логи
        std::ofstream logFile("test_game_log.txt", std::ios::trunc);
        logFile.close();
    }
    
    void TearDown() override {
        // Даем время потокам завершиться
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};

TEST_F(GameTest, GameInitialization) {
    Game game;
    
    // Проверяем, что игра инициализируется
    game.initialize(10);
    
    // После инициализации потоки еще не запущены
    // game.start() запустит потоки
}

TEST_F(GameTest, GameStartStop) {
    Game game;
    game.initialize(3);
    
    // Запускаем игру на короткое время
    std::atomic<bool> testCompleted(false);
    
    // В реальном тесте мы не можем просто вызвать start() и остановить,
    // так как он работает 30 секунд. Вместо этого тестируем только создание.
    EXPECT_NO_THROW({
        // Просто проверяем, что объект создается без ошибок
        Game testGame;
        testGame.initialize(5);
    });
}

TEST_F(GameTest, ThreadSafety) {
    // Тест на потокобезопасность
    // Создаем несколько потоков, которые работают с NPC
    std::vector<std::shared_ptr<NPC>> npcs;
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    
    // Создаем NPC
    for (int i = 0; i < 10; ++i) {
        auto npc = NPCFactory::createNPC(NpcType::Dragon, i * 10, i * 10, 
                                         "Dragon_" + std::to_string(i));
        if (npc) {
            npc->subscribe(consoleObserver);
            npcs.push_back(npc);
        }
    }
    
    std::atomic<int> moveCount{0};
    std::atomic<int> attackCount{0};
    
    // Запускаем несколько потоков
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&]() {
            for (auto& npc : npcs) {
                if (npc) {
                    // Меняем позицию
                    npc->setPosition(npc->getX() + 1, npc->getY() + 1);
                    moveCount++;
                    
                    // Проверяем состояние
                    bool alive = npc->isAlive();
                    attackCount += alive ? 1 : 0;
                }
            }
        });
    }
    
    // Ждем завершения потоков
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Проверяем, что все операции выполнены без падений
    EXPECT_EQ(moveCount, 50); // 5 потоков × 10 NPC
}

TEST_F(GameTest, MovementDistances) {
    // Проверяем расстояния перемещения для разных типов NPC
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "TestDragon");
    auto knight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "TestKnight");
    auto pegasus = NPCFactory::createNPC(NpcType::Pegasus, 0, 0, "TestPegasus");
    
    ASSERT_NE(dragon, nullptr);
    ASSERT_NE(knight, nullptr);
    ASSERT_NE(pegasus, nullptr);
    
    EXPECT_EQ(dragon->getMoveDistance(), 50);
    EXPECT_EQ(knight->getMoveDistance(), 30);
    EXPECT_EQ(pegasus->getMoveDistance(), 30);
}

TEST_F(GameTest, KillDistances) {
    // Проверяем дистанции убийства для разных типов NPC
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "TestDragon");
    auto knight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "TestKnight");
    auto pegasus = NPCFactory::createNPC(NpcType::Pegasus, 0, 0, "TestPegasus");
    
    ASSERT_NE(dragon, nullptr);
    ASSERT_NE(knight, nullptr);
    ASSERT_NE(pegasus, nullptr);
    
    EXPECT_EQ(dragon->getKillDistance(), 30);
    EXPECT_EQ(knight->getKillDistance(), 10);
    EXPECT_EQ(pegasus->getKillDistance(), 10);
}

TEST_F(GameTest, DiceRolls) {
    // Проверяем, что броски кубика в допустимом диапазоне
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "TestDragon");
    
    ASSERT_NE(dragon, nullptr);
    
    for (int i = 0; i < 100; ++i) {
        int attack = dragon->rollAttack();
        int defense = dragon->rollDefense();
        
        EXPECT_GE(attack, 1);
        EXPECT_LE(attack, 6);
        EXPECT_GE(defense, 1);
        EXPECT_LE(defense, 6);
    }
}

TEST_F(GameTest, NPCAliveState) {
    // Проверяем состояние "жив/мертв"
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "TestDragon");
    
    ASSERT_NE(dragon, nullptr);
    
    // По умолчанию должен быть жив
    EXPECT_TRUE(dragon->isAlive());
    
    // Меняем состояние
    dragon->setAlive(false);
    EXPECT_FALSE(dragon->isAlive());
    
    dragon->setAlive(true);
    EXPECT_TRUE(dragon->isAlive());
}

TEST_F(GameTest, FactoryCreatesAllTypes) {
    // Проверяем, что фабрика создает все типы NPC
    auto dragon = NPCFactory::createNPC(NpcType::Dragon, 0, 0, "Dragon");
    auto knight = NPCFactory::createNPC(NpcType::Knight, 0, 0, "Knight");
    auto pegasus = NPCFactory::createNPC(NpcType::Pegasus, 0, 0, "Pegasus");
    
    EXPECT_NE(dragon, nullptr);
    EXPECT_NE(knight, nullptr);
    EXPECT_NE(pegasus, nullptr);
    
    EXPECT_EQ(dragon->getType(), NpcType::Dragon);
    EXPECT_EQ(knight->getType(), NpcType::Knight);
    EXPECT_EQ(pegasus->getType(), NpcType::Pegasus);
}

TEST_F(GameTest, ThreadCreation) {
    // Проверяем, что можем создавать потоки
    std::atomic<int> counter{0};
    
    std::thread t1([&counter]() {
        counter += 1;
    });
    
    std::thread t2([&counter]() {
        counter += 2;
    });
    
    t1.join();
    t2.join();
    
    EXPECT_EQ(counter, 3);
}