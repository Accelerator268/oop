#include "knight.h"
#include "visitor.h"

Knight::Knight(int x, int y, const std::string& name) 
    : NPC(NpcType::Knight, x, y, name) {}

Knight::Knight(std::istream& is) 
    : NPC(NpcType::Knight, 0, 0, "") {
    load(is);
}

bool Knight::accept(const std::shared_ptr<IFightVisitor>& visitor, 
                   const std::shared_ptr<NPC>& attacker) {
    return visitor->visit(std::dynamic_pointer_cast<Knight>(shared_from_this()), attacker);
}

void Knight::print(std::ostream& os) const {
    os << "Knight '" << name << "' at (" << x << ", " << y << ")";
}

void Knight::save(std::ostream& os) const {
    NPC::save(os);
}

void Knight::load(std::istream& is) {
    NPC::load(is);
}