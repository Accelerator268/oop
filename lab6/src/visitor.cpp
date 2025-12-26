#include "visitor.h"
#include "dragon.h"
#include "knight.h"
#include "pegasus.h"
#include <iostream>

//Правила сражений:
//1. Пегас никого не трогает (всегда false)
//2. Дракон ест пегаса
//3. Рыцарь убивает дракона
//4. В остальных случаях - ничья (false)

bool FightVisitor::visit(const std::shared_ptr<Dragon>& defender,
                        const std::shared_ptr<NPC>& attacker) {
    //Проверяем тип атакующего
    if (attacker->getType() == NpcType::Knight) {
        //Рыцарь убивает дракона
        std::cout << "Knight kills Dragon!" << std::endl;
        return true;
    }
    return false;
}

bool FightVisitor::visit(const std::shared_ptr<Knight>& defender,
                        const std::shared_ptr<NPC>& attacker) {
    //Пегас не трогает рыцаря, дракон не ест рыцаря
    return false;
}

bool FightVisitor::visit(const std::shared_ptr<Pegasus>& defender,
                        const std::shared_ptr<NPC>& attacker) {
    //Проверяем тип атакующего
    if (attacker->getType() == NpcType::Dragon) {
        //Дракон ест пегаса
        std::cout << "Dragon eats Pegasus!" << std::endl;
        return true;
    }
    return false;
}