#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

//Объявления
class Dragon;
class Knight;
class Pegasus;
class IFightVisitor;
class IFightObserver;

//NPC типы
enum class NpcType {
    Unknown = 0,
    Dragon = 1,
    Knight = 2,
    Pegasus = 3
};

class NPC : public std::enable_shared_from_this<NPC> {
protected:
    NpcType type;
    int x;
    int y;
    std::string name;
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int x, int y, const std::string& name);
    virtual ~NPC() = default;

    //Геттеры
    NpcType getType() const { return type; }
    int getX() const { return x; }
    int getY() const { return y; }
    const std::string& getName() const { return name; }

    //Сеттеры
    void setPosition(int newX, int newY);
    void setName(const std::string& newName);

    //паттерн обзервера
    void subscribe(const std::shared_ptr<IFightObserver>& observer);
    void notifyFight(const std::shared_ptr<NPC>& defender, bool win);

    //Патерн визитора
    virtual bool accept(const std::shared_ptr<IFightVisitor>& visitor, 
                       const std::shared_ptr<NPC>& attacker) = 0;

    //Проверка на близость к другому NPC
    bool isClose(const std::shared_ptr<NPC>& other, int distance) const;

    //Вывод информации
    virtual void print(std::ostream& os) const = 0;

    //Сохранить/загрузить
    virtual void save(std::ostream& os) const;
    virtual void load(std::istream& is);
};

std::ostream& operator<<(std::ostream& os, const NPC& npc);