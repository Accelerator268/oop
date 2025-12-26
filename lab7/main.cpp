#include <iostream>
#include "game.h"
#include "observer.h"

// Глобальные observers
static auto consoleObserver = std::make_shared<ConsoleObserver>();
static auto fileObserver = std::make_shared<FileObserver>("game_log.txt");

int main() {
    try {
        Game game;
        
        std::cout << "=== DUNGEON SIMULATOR ===" << std::endl;
        std::cout << "Initializing game with 50 NPCs..." << std::endl;
        
        game.initialize(50);
        game.start();
        
        std::cout << "\nSimulation completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}