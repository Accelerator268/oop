#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include "npc.h"

struct BattleTask {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class Game {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    std::vector<std::thread> threads;
    std::atomic<bool> running;
    
    // Потоки
    std::thread movementThread;
    std::thread battleThread;
    std::thread mapThread;
    
    // Очередь задач для боя
    std::vector<BattleTask> battleQueue;
    std::mutex battleMutex;
    std::condition_variable battleCV;
    
    // Мьютекс для вывода
    static std::mutex coutMutex;
    
    // Константы игры
    static constexpr int MAP_WIDTH = 500;
    static constexpr int MAP_HEIGHT = 500;
    static constexpr int GAME_DURATION = 30; // секунды
    
public:
    Game();
    ~Game();
    
    void initialize(int npcCount = 50);
    void start();
    void stop();
    
private:
    void movementWorker();
    void battleWorker();
    void mapWorker();
    
    static void safePrint(const std::string& message);
    void printMap() const;
    
    void addBattleTask(const BattleTask& task);
    BattleTask getBattleTask();
};