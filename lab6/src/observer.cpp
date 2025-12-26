#include "observer.h"
#include "npc.h"
#include <iostream>

// ConsoleObserver
void ConsoleObserver::onFight(const std::shared_ptr<NPC>& attacker,
                             const std::shared_ptr<NPC>& defender,
                             bool win) {
    if (win) {
        std::cout << "\nBATTLE RESULT" << std::endl;
        std::cout << "Attacker: ";
        attacker->print(std::cout);
        std::cout << std::endl;
        std::cout << "Defender: ";
        defender->print(std::cout);
        std::cout << " was killed!" << std::endl;
        std::cout << "\n" << std::endl;
    }
}

//FileObserver
FileObserver::FileObserver(const std::string& filename) {
    logFile.open(filename, std::ios::app);
    if (logFile.is_open()) {
        logFile << "Battle Log" << std::endl;
    }
}

FileObserver::~FileObserver() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void FileObserver::onFight(const std::shared_ptr<NPC>& attacker,
                          const std::shared_ptr<NPC>& defender,
                          bool win) {
    if (win && logFile.is_open()) {
        logFile << "Battle: ";
        attacker->print(logFile);
        logFile << " killed ";
        defender->print(logFile);
        logFile << std::endl;
    }
}