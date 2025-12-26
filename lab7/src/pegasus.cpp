#include "pegasus.h"
#include "visitor.h"

Pegasus::Pegasus(int x, int y, const std::string& name) 
    : NPC(NpcType::Pegasus, x, y, name) {}

Pegasus::Pegasus(std::istream& is) 
    : NPC(NpcType::Pegasus, 0, 0, "") {
    load(is);
}

int Pegasus::getMoveDistance() const {
    return 30;  // Пегас перемещается на 30 метров
}

int Pegasus::getKillDistance() const {
    return 10;  // Дистанция убийства пегаса
}

bool Pegasus::accept(const std::shared_ptr<IFightVisitor>& visitor, 
                    const std::shared_ptr<NPC>& attacker) {
    return visitor->visit(std::dynamic_pointer_cast<Pegasus>(shared_from_this()), attacker);
}

void Pegasus::print(std::ostream& os) const {
    os << "Pegasus '" << name << "' at (" << x << ", " << y << ")";
}

void Pegasus::save(std::ostream& os) const {
    NPC::save(os);
}

void Pegasus::load(std::istream& is) {
    NPC::load(is);
}