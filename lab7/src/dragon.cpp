#include "dragon.h"
#include "visitor.h"

Dragon::Dragon(int x, int y, const std::string& name) 
    : NPC(NpcType::Dragon, x, y, name) {}

Dragon::Dragon(std::istream& is) 
    : NPC(NpcType::Dragon, 0, 0, "") {
    load(is);
}

int Dragon::getMoveDistance() const {
    return 50;  // Дракон перемещается на 50 метров
}

int Dragon::getKillDistance() const {
    return 30;  // Дистанция убийства дракона
}

bool Dragon::accept(const std::shared_ptr<IFightVisitor>& visitor, 
                   const std::shared_ptr<NPC>& attacker) {
    return visitor->visit(std::dynamic_pointer_cast<Dragon>(shared_from_this()), attacker);
}

void Dragon::print(std::ostream& os) const {
    os << "Dragon '" << name << "' at (" << x << ", " << y << ")";
}

void Dragon::save(std::ostream& os) const {
    NPC::save(os);
}

void Dragon::load(std::istream& is) {
    NPC::load(is);
}