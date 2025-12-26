#include "visitor.h"
#include "dragon.h"
#include "knight.h"
#include "pegasus.h"

bool FightVisitor::visit(const std::shared_ptr<Dragon>& defender,
                        const std::shared_ptr<NPC>& attacker) {
    // Проверяем тип атакующего
    if (attacker->getType() == NpcType::Knight) {
        // Рыцарь может убивать дракона
        return true;
    }
    // Дракон не может убивать дракона, пегас не атакует дракона
    return false;
}

bool FightVisitor::visit(const std::shared_ptr<Knight>& defender,
                        const std::shared_ptr<NPC>& attacker) {
    // Никто не может атаковать рыцаря согласно правилам
    // (Дракон не ест рыцаря, пегас никого не трогает, рыцарь не атакует рыцаря)
    return false;
}

bool FightVisitor::visit(const std::shared_ptr<Pegasus>& defender,
                        const std::shared_ptr<NPC>& attacker) {
    // Проверяем тип атакующего
    if (attacker->getType() == NpcType::Dragon) {
        // Дракон может есть пегаса
        return true;
    }
    // Рыцарь не атакует пегаса, пегас не атакует пегаса
    return false;
}