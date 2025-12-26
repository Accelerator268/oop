#pragma once
#include "npc.h"
#include "visitor.h"

class Dragon : public NPC {
public:
    Dragon(int x = 0, int y = 0, const std::string& name = "Dragon");
    Dragon(std::istream& is);

    bool accept(const std::shared_ptr<IFightVisitor>& visitor, 
               const std::shared_ptr<NPC>& attacker) override;

    void print(std::ostream& os) const override;
    void save(std::ostream& os) const override;
    void load(std::istream& is) override;
};