#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <memory>
#include "observer.h"
#include "dragon.h"
#include "knight.h"

class MockObserver : public IFightObserver {
public:
    int fightCount = 0;
    std::shared_ptr<NPC> lastAttacker;
    std::shared_ptr<NPC> lastDefender;
    bool lastWin = false;
    
    void onFight(const std::shared_ptr<NPC>& attacker,
                const std::shared_ptr<NPC>& defender,
                bool win) override {
        ++fightCount;
        lastAttacker = attacker;
        lastDefender = defender;
        lastWin = win;
    }
};

class ObserverTest : public ::testing::Test {
protected:
    void SetUp() override {
        dragon = std::make_shared<Dragon>(0, 0, "TestDragon");
        knight = std::make_shared<Knight>(0, 0, "TestKnight");
        mockObserver = std::make_shared<MockObserver>();
        
        // Очищаем файл лога перед тестами
        std::ofstream logFile("test_log.txt", std::ios::trunc);
        logFile.close();
    }
    
    std::shared_ptr<Dragon> dragon;
    std::shared_ptr<Knight> knight;
    std::shared_ptr<MockObserver> mockObserver;
};

TEST_F(ObserverTest, ConsoleObserver) {
    ConsoleObserver observer;
    
    // Просто проверяем, что метод не падает
    EXPECT_NO_THROW(
        observer.onFight(dragon, knight, true)
    );
    
    EXPECT_NO_THROW(
        observer.onFight(knight, dragon, false)
    );
}

TEST_F(ObserverTest, FileObserver) {
    {
        FileObserver observer("test_log.txt");
        
        observer.onFight(dragon, knight, true);
        observer.onFight(knight, dragon, false);
        observer.onFight(dragon, knight, true);
    }
    
    // Проверяем содержимое файла
    std::ifstream logFile("test_log.txt");
    ASSERT_TRUE(logFile.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(logFile, line)) {
        ++lineCount;
        if (lineCount > 1) { // Пропускаем заголовок
            EXPECT_NE(line.find("Battle:"), std::string::npos);
        }
    }
    
    EXPECT_GE(lineCount, 3); // Заголовок + 2 события (только win=true)
    logFile.close();
}

TEST_F(ObserverTest, SubscribeAndNotify) {
    // Подписываем NPC на observer
    dragon->subscribe(mockObserver);
    knight->subscribe(mockObserver);
    
    // Уведомляем о битве
    dragon->notifyFight(knight, true);
    
    EXPECT_EQ(mockObserver->fightCount, 1);
    EXPECT_EQ(mockObserver->lastAttacker, dragon);
    EXPECT_EQ(mockObserver->lastDefender, knight);
    EXPECT_TRUE(mockObserver->lastWin);
    
    // Еще одно уведомление
    knight->notifyFight(dragon, false);
    
    EXPECT_EQ(mockObserver->fightCount, 2);
    EXPECT_EQ(mockObserver->lastAttacker, knight);
    EXPECT_EQ(mockObserver->lastDefender, dragon);
    EXPECT_FALSE(mockObserver->lastWin);
}

TEST_F(ObserverTest, MultipleObservers) {
    auto mockObserver2 = std::make_shared<MockObserver>();
    
    // Подписываем на двух observers
    dragon->subscribe(mockObserver);
    dragon->subscribe(mockObserver2);
    
    // Уведомляем
    dragon->notifyFight(knight, true);
    
    EXPECT_EQ(mockObserver->fightCount, 1);
    EXPECT_EQ(mockObserver2->fightCount, 1);
}

TEST_F(ObserverTest, FileObserverConstructorDestructor) {
    {
        FileObserver observer1("test_log.txt");
        observer1.onFight(dragon, knight, true);
    }
    
    {
        // Новый observer должен дописывать в существующий файл
        FileObserver observer2("test_log.txt");
        observer2.onFight(knight, dragon, true);
    }
    
    // Проверяем, что файл содержит обе записи
    std::ifstream logFile("test_log.txt");
    std::string content((std::istreambuf_iterator<char>(logFile)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_NE(content.find("TestDragon"), std::string::npos);
    EXPECT_NE(content.find("TestKnight"), std::string::npos);
    logFile.close();
}