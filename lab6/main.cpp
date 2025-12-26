#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <fstream>
#include <limits>
#include <cstdlib>
#include <ctime>

#include "npc.h"
#include "factory.h"
#include "visitor.h"
#include "observer.h"

using NPCSet = std::set<std::shared_ptr<NPC>>;


//Глобальные observers (или можно сделать их статическими в классе)
static auto consoleObserver = std::make_shared<ConsoleObserver>();
static auto fileObserver = std::make_shared<FileObserver>("battle_log.txt");


//Сохранение всех NPC в файл
void saveNPCs(const NPCSet& npcs, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        return;
    }
    
    //Сохраняем количество NPC
    file << npcs.size() << std::endl << std::endl;
    
    //Сохраняем каждого NPC
    for (const auto& npc : npcs) {
        npc->save(file);
    }
    
    file.close();
    std::cout << "Successfully saved " << npcs.size() << " NPCs to " << filename << std::endl;
}

//Загрузка NPC из файла
NPCSet loadNPCs(const std::string& filename) {
    NPCSet npcs;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for reading" << std::endl;
        return npcs;
    }
    
    //Читаем количество NPC
    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Error reading NPC count from file" << std::endl;
        return npcs;
    }
    
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    int count;
    try {
        count = std::stoi(line);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing NPC count: '" << line << "'" << std::endl;
        return npcs;
    }
    
    //Пропускаем пустую строку после количества
    std::getline(file, line);
    
    std::cout << "Loading " << count << " NPCs from " << filename << "..." << std::endl;
    
    //Загружаем каждого NPC
    int loadedCount = 0;
    for (int i = 0; i < count; ++i) {
        auto npc = NPCFactory::loadNPC(file);
        if (npc) {
            //Подписываем на observers
            npc->subscribe(consoleObserver);
            npc->subscribe(fileObserver);
            npcs.insert(npc);
            loadedCount++;
            std::cout << "  Loaded: " << *npc << std::endl;
        } else {
            std::cerr << "  Failed to load NPC #" << (i + 1) << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << loadedCount << " out of " << count << " NPCs" << std::endl;
    return npcs;
}

//Боевой режим с использованием Visitor
NPCSet battle(NPCSet& npcs, int distance) {
    NPCSet killed;
    auto visitor = std::make_shared<FightVisitor>();
    
    for (const auto& attacker : npcs) {
        for (const auto& defender : npcs) {
            if (attacker != defender && 
                attacker->isClose(defender, distance) &&
                killed.find(defender) == killed.end() &&
                killed.find(attacker) == killed.end()) {
                
                bool result = defender->accept(visitor, attacker);
                if (result) {
                    //Уведомляем о победе
                    attacker->notifyFight(defender, true);
                    killed.insert(defender);
                    
                    //Проверяем, может ли атакующий тоже погибнуть
                    if (attacker->getType() == NpcType::Dragon && 
                        defender->getType() == NpcType::Knight) {
                        //Если дракон атаковал рыцаря, дракон тоже умирает
                        killed.insert(attacker);
                    }
                }
            }
        }
    }
    
    return killed;
}

//Печать всех NPC
void printNPCs(const NPCSet& npcs) {
    std::cout << "\nNPC List (" << npcs.size() << " total)" << std::endl;
    for (const auto& npc : npcs) {
        std::cout << "- " << *npc << std::endl;
    }
    std::cout << "\n" << std::endl;
}

//Основное меню
void showMenu() {
    std::cout << "\nDungeon Editor" << std::endl;
    std::cout << "1. Add NPC" << std::endl;
    std::cout << "2. Remove NPC" << std::endl;
    std::cout << "3. Print all NPCs" << std::endl;
    std::cout << "4. Save NPCs to file" << std::endl;
    std::cout << "5. Load NPCs from file" << std::endl;
    std::cout << "6. Start battle mode" << std::endl;
    std::cout << "7. Generate random NPCs" << std::endl;
    std::cout << "8. Clear all NPCs" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Choice: ";
}

int main() {
    std::srand(std::time(nullptr));
    NPCSet npcs;
    
    
    int choice;
    
    do {
        showMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: { //Добавить NPC
                std::cout << "\nAdd NPC" << std::endl;
                std::cout << "NPC types: 1-Dragon, 2-Knight, 3-Pegasus" << std::endl;
                std::cout << "Enter type (1-3): ";
                int typeInt;
                std::cin >> typeInt;
                
                std::cout << "Enter X coordinate (0-500): ";
                int x;
                std::cin >> x;
                
                std::cout << "Enter Y coordinate (0-500): ";
                int y;
                std::cin >> y;
                
                std::cin.ignore();
                std::cout << "Enter name: ";
                std::string name;
                std::getline(std::cin, name);
                
                NpcType type = static_cast<NpcType>(typeInt);
                auto npc = NPCFactory::createNPC(type, x, y, name);
                
                if (npc) {
                    // Подписываемся на observers
                    npc->subscribe(consoleObserver);
                    npc->subscribe(fileObserver);
                    npcs.insert(npc);
                    std::cout << "NPC added successfully!" << std::endl;
                }
                break;
            }
            
            case 2: { // Удалить NPC
                std::cout << "\nRemove NPC" << std::endl;
                if (npcs.empty()) {
                    std::cout << "No NPCs to remove." << std::endl;
                    break;
                }
                
                printNPCs(npcs);
                std::cout << "Enter NPC index to remove (1-" << npcs.size() << "): ";
                int index;
                std::cin >> index;
                
                if (index > 0 && index <= npcs.size()) {
                    auto it = npcs.begin();
                    std::advance(it, index - 1);
                    npcs.erase(it);
                    std::cout << "NPC removed." << std::endl;
                } else {
                    std::cout << "Invalid index!" << std::endl;
                }
                break;
            }
            
            case 3: //Печать всех NPC
                printNPCs(npcs);
                break;
                
            case 4: { //Сохранение в файл
                std::cout << "\nEnter filename to save: ";
                std::string filename;
                std::getline(std::cin, filename);
                saveNPCs(npcs, filename);
                break;
            }
            
            case 5: { //Загрузка из файла
                std::cout << "\nEnter filename to load: ";
                std::string filename;
                std::getline(std::cin, filename);
                
                auto loaded = loadNPCs(filename);
                //Подписываем новых NPC на observers
                for (auto& npc : loaded) {
                    npc->subscribe(consoleObserver);
                    npc->subscribe(fileObserver);
                }
                npcs.insert(loaded.begin(), loaded.end());
                break;
            }
            
            case 6: { //Боевой режим
                std::cout << "\nBattle Mode" << std::endl;
                if (npcs.size() < 2) {
                    std::cout << "Need at least 2 NPCs for battle!" << std::endl;
                    break;
                }
                
                std::cout << "Enter battle distance (meters): ";
                int distance;
                std::cin >> distance;
                
                std::cout << "\nStarting battle with " << npcs.size() << " NPCs..." << std::endl;
                auto killed = battle(npcs, distance);
                
                //Удаляем убитых
                for (const auto& dead : killed) {
                    npcs.erase(dead);
                }
                
                std::cout << "Battle finished. " << killed.size() << " NPCs killed." << std::endl;
                std::cout << npcs.size() << " NPCs survived." << std::endl;
                break;
            }
            
            case 7: { //Генерация случайных NPC
                std::cout << "\nHow many NPCs to generate? ";
                int count;
                std::cin >> count;
                
                for (int i = 0; i < count; ++i) {
                    NpcType type = static_cast<NpcType>((std::rand() % 3) + 1);
                    int x = std::rand() % 501;
                    int y = std::rand() % 501;
                    
                    std::string name = NPCFactory::getStringFromType(type) + 
                                      "_" + std::to_string(i+1);
                    
                    auto npc = NPCFactory::createNPC(type, x, y, name);
                    if (npc) {
                        npc->subscribe(consoleObserver);
                        npc->subscribe(fileObserver);
                        npcs.insert(npc);
                    }
                }
                std::cout << "Generated " << count << " random NPCs." << std::endl;
                break;
            }
            
            case 8: //Очистить всех NPC
                npcs.clear();
                std::cout << "All NPCs cleared." << std::endl;
                break;
                
            case 0: //Выход
                std::cout << "Goodbye!" << std::endl;
                break;
                
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
        
    } while (choice != 0);
    
    return 0;
}