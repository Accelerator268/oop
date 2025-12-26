#include "game.h"
#include "factory.h"
#include "visitor.h"
#include "observer.h"
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#include <sstream>

std::mutex Game::coutMutex;

Game::Game() : running(false) {
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    auto fileObserver = std::make_shared<FileObserver>("game_log.txt");
}

Game::~Game() {
    stop();
}

void Game::initialize(int npcCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(1, 3);
    std::uniform_int_distribution<> coordDist(0, 499);
    
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    auto fileObserver = std::make_shared<FileObserver>("game_log.txt");
    
    for (int i = 0; i < npcCount; ++i) {
        NpcType type = static_cast<NpcType>(typeDist(gen));
        int x = coordDist(gen);
        int y = coordDist(gen);
        
        std::string name = NPCFactory::getStringFromType(type) + 
                          "_" + std::to_string(i+1);
        
        auto npc = NPCFactory::createNPC(type, x, y, name);
        if (npc) {
            npc->subscribe(consoleObserver);
            npc->subscribe(fileObserver);
            npcs.push_back(npc);
        }
    }
    
    safePrint("Game initialized with " + std::to_string(npcs.size()) + " NPCs");
}

void Game::start() {
    running = true;
    
    // Запускаем потоки
    movementThread = std::thread(&Game::movementWorker, this);
    battleThread = std::thread(&Game::battleWorker, this);
    mapThread = std::thread(&Game::mapWorker, this);
    
    safePrint("Game started! Duration: " + std::to_string(GAME_DURATION) + " seconds");
    
    // Ждем завершения игры
    std::this_thread::sleep_for(std::chrono::seconds(GAME_DURATION));
    stop();
    
    // Выводим список выживших
    safePrint("\n=== GAME OVER ===");
    safePrint("Survivors (" + std::to_string(npcs.size()) + "):");
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            std::stringstream ss;
            ss << *npc;
            safePrint("  " + ss.str());
        }
    }
}

void Game::stop() {
    running = false;
    
    // Оповещаем потоки
    battleCV.notify_all();
    
    // Ждем завершения потоков
    if (movementThread.joinable()) movementThread.join();
    if (battleThread.joinable()) battleThread.join();
    if (mapThread.joinable()) mapThread.join();
    
    // Удаляем мертвых NPC
    npcs.erase(
        std::remove_if(npcs.begin(), npcs.end(),
            [](const std::shared_ptr<NPC>& npc) { return !npc->isAlive(); }),
        npcs.end()
    );
}

void Game::movementWorker() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dirDist(-1, 1);
    
    while (running) {
        for (auto& npc : npcs) {
            if (!npc->isAlive()) continue;
            
            // Генерируем случайное направление
            int dx = dirDist(gen);
            int dy = dirDist(gen);
            
            // Получаем текущую позицию
            int currentX = npc->getX();
            int currentY = npc->getY();
            
            // Получаем максимальное расстояние перемещения
            int moveDist = npc->getMoveDistance();
            
            // Вычисляем новую позицию с учетом направления и расстояния
            int newX = currentX + dx * moveDist;
            int newY = currentY + dy * moveDist;
            
            // Ограничиваем границами карты
            newX = std::max(0, std::min(MAP_WIDTH - 1, newX));
            newY = std::max(0, std::min(MAP_HEIGHT - 1, newY));
            
            // Обновляем позицию
            npc->setPosition(newX, newY);
            
            // Проверяем ближайших NPC для боя
            int killDist = npc->getKillDistance();
            for (auto& other : npcs) {
                if (other == npc || !other->isAlive()) continue;
                
                if (npc->isClose(other, killDist)) {
                    // Проверяем правила боя через Visitor
                    auto visitor = std::make_shared<FightVisitor>();
                    if (other->accept(visitor, npc)) {
                        // Создаем задачу для боя
                        BattleTask task{npc, other};
                        addBattleTask(task);
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Game::battleWorker() {
    auto visitor = std::make_shared<FightVisitor>();
    
    while (running) {
        BattleTask task;
        {
            std::unique_lock lock(battleMutex);
            battleCV.wait_for(lock, std::chrono::milliseconds(100),
                [this]() { return !battleQueue.empty() || !running; });
            
            if (!running && battleQueue.empty()) break;
            
            if (!battleQueue.empty()) {
                task = battleQueue.front();
                battleQueue.erase(battleQueue.begin());
            }
        }
        
        if (task.attacker && task.defender && 
            task.attacker->isAlive() && task.defender->isAlive()) {
            
            // Каждый NPC бросает кубик
            int attackPower = task.attacker->rollAttack();
            int defensePower = task.defender->rollDefense();
            
            if (attackPower > defensePower) {
                // Убийство
                task.defender->setAlive(false);
                
                // Уведомляем о победе
                task.attacker->notifyFight(task.defender, true);
                
                std::stringstream ss;
                ss << task.attacker->getName() << " killed " 
                   << task.defender->getName() 
                   << " (Attack: " << attackPower 
                   << " vs Defense: " << defensePower << ")";
                safePrint(ss.str());
            } else {
                std::stringstream ss;
                ss << task.attacker->getName() << " failed to kill " 
                   << task.defender->getName()
                   << " (Attack: " << attackPower 
                   << " vs Defense: " << defensePower << ")";
                safePrint(ss.str());
            }
        }
    }
}

void Game::mapWorker() {
    while (running) {
        printMap();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Game::addBattleTask(const BattleTask& task) {
    std::lock_guard lock(battleMutex);
    battleQueue.push_back(task);
    battleCV.notify_one();
}

BattleTask Game::getBattleTask() {
    std::lock_guard lock(battleMutex);
    if (battleQueue.empty()) {
        return {nullptr, nullptr};
    }
    BattleTask task = battleQueue.front();
    battleQueue.erase(battleQueue.begin());
    return task;
}

void Game::safePrint(const std::string& message) {
    std::lock_guard lock(coutMutex);
    std::cout << message << std::endl;
}

void Game::printMap() const {
    std::lock_guard lock(coutMutex);
    std::cout << "\n=== CURRENT MAP ===" << std::endl;
    
    // Создаем простую текстовую карту
    const int gridSize = 10;
    char grid[gridSize][gridSize];
    
    // Инициализируем пустую карту
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            grid[i][j] = '.';
        }
    }
    
    // Размещаем живых NPC на карте
    int aliveCount = 0;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            aliveCount++;
            
            // Масштабируем координаты к размеру сетки
            int gridX = (npc->getX() * gridSize) / MAP_WIDTH;
            int gridY = (npc->getY() * gridSize) / MAP_HEIGHT;
            
            gridX = std::min(gridSize - 1, std::max(0, gridX));
            gridY = std::min(gridSize - 1, std::max(0, gridY));
            
            char symbol;
            switch (npc->getType()) {
                case NpcType::Dragon: symbol = 'D'; break;
                case NpcType::Knight: symbol = 'K'; break;
                case NpcType::Pegasus: symbol = 'P'; break;
                default: symbol = '?';
            }
            
            grid[gridY][gridX] = symbol;
        }
    }
    
    // Выводим карту
    for (int i = 0; i < gridSize; ++i) {
        std::cout << "|";
        for (int j = 0; j < gridSize; ++j) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << "|" << std::endl;
    }
    
    std::cout << "Alive: " << aliveCount << std::endl;
}