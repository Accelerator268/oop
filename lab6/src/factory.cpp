#include "factory.h"
#include "dragon.h"
#include "knight.h"
#include "pegasus.h"
#include <iostream>
#include <sstream>

std::shared_ptr<NPC> NPCFactory::createNPC(NpcType type, int x, int y, const std::string& name) {
    switch (type) {
        case NpcType::Dragon:
            return std::make_shared<Dragon>(x, y, name.empty() ? "Dragon" : name);
        case NpcType::Knight:
            return std::make_shared<Knight>(x, y, name.empty() ? "Knight" : name);
        case NpcType::Pegasus:
            return std::make_shared<Pegasus>(x, y, name.empty() ? "Pegasus" : name);
        default:
            std::cerr << "Error: Unknown NPC type: " << static_cast<int>(type) << std::endl;
            return nullptr;
    }
}

std::shared_ptr<NPC> NPCFactory::loadNPC(std::istream& is) {
    //Читаем тип NPC
    std::string line;
    if (!std::getline(is, line)) {
        std::cerr << "Error reading NPC type from file" << std::endl;
        return nullptr;
    }
    
    // Удаляем возможные пробелы
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    if (line.empty()) {
        //Пустая строка, попробуем еще раз
        if (!std::getline(is, line)) {
            return nullptr;
        }
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
    }
    
    //Преобразуем строку в число
    std::stringstream ss(line);
    int typeInt;
    if (!(ss >> typeInt)) {
        std::cerr << "Error parsing NPC type: '" << line << "'" << std::endl;
        return nullptr;
    }
    
    NpcType type = static_cast<NpcType>(typeInt);
    
    //Создаем NPC и загружаем его данные
    auto npc = createNPC(type, 0, 0, "");
    if (!npc) {
        std::cerr << "Failed to create NPC of type: " << typeInt << std::endl;
        return nullptr;
    }
    
    //Загружаем данные NPC
    try {
        npc->load(is);
    } catch (const std::exception& e) {
        std::cerr << "Error loading NPC data: " << e.what() << std::endl;
        return nullptr;
    }
    
    return npc;
}

NpcType NPCFactory::getTypeFromString(const std::string& typeStr) {
    if (typeStr == "Dragon" || typeStr == "dragon" || typeStr == "1") return NpcType::Dragon;
    if (typeStr == "Knight" || typeStr == "knight" || typeStr == "2") return NpcType::Knight;
    if (typeStr == "Pegasus" || typeStr == "pegasus" || typeStr == "3") return NpcType::Pegasus;
    return NpcType::Unknown;
}

std::string NPCFactory::getStringFromType(NpcType type) {
    switch (type) {
        case NpcType::Dragon: return "Dragon";
        case NpcType::Knight: return "Knight";
        case NpcType::Pegasus: return "Pegasus";
        default: return "Unknown";
    }
}