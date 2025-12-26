#pragma once
#include "npc.h"
#include "visitor.h"

class Knight : public NPC {
public:
    Knight(int x = 0, int y = 0, const std::string& name = "Knight");
    Knight(std::istream& is);

    bool accept(const std::shared_ptr<IFightVisitor>& visitor, 
               const std::shared_ptr<NPC>& attacker) override;

    void print(std::ostream& os) const override;
    void save(std::ostream& os) const override;
    void load(std::istream& is) override;
};