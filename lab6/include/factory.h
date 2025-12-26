#pragma once
#include <memory>
#include "npc.h"

class NPCFactory {
public:
    static std::shared_ptr<NPC> createNPC(NpcType type, 
                                         int x = 0, 
                                         int y = 0, 
                                         const std::string& name = "");
    
    static std::shared_ptr<NPC> loadNPC(std::istream& is);
    
    static NpcType getTypeFromString(const std::string& typeStr);
    static std::string getStringFromType(NpcType type);
};