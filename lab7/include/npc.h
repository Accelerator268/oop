#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <mutex>
#include <shared_mutex>

// Объявления
class Dragon;
class Knight;
class Pegasus;
class IFightVisitor;
class IFightObserver;

// NPC типы
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
    bool alive;
    mutable std::shared_mutex mutex;  // Используем shared_mutex
    
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int x, int y, const std::string& name);
    virtual ~NPC() = default;

    // Геттеры
    NpcType getType() const;
    int getX() const;
    int getY() const;
    const std::string& getName() const;
    bool isAlive() const;

    // Сеттеры
    void setPosition(int newX, int newY);
    void setName(const std::string& newName);
    void setAlive(bool isAlive);

    // Методы для движения и боя
    virtual int getMoveDistance() const = 0;
    virtual int getKillDistance() const = 0;
    int rollAttack() const;
    int rollDefense() const;

    // Паттерн обзервера
    void subscribe(const std::shared_ptr<IFightObserver>& observer);
    void notifyFight(const std::shared_ptr<NPC>& defender, bool win);

    // Паттерн визитора
    virtual bool accept(const std::shared_ptr<IFightVisitor>& visitor,
                       const std::shared_ptr<NPC>& attacker) = 0;

    // Проверка на близость к другому NPC
    bool isClose(const std::shared_ptr<NPC>& other, int distance) const;

    // Вывод информации
    virtual void print(std::ostream& os) const = 0;

    // Сохранить/загрузить
    virtual void save(std::ostream& os) const;
    virtual void load(std::istream& is);

    // Блокировка для потокобезопасности
    std::unique_lock<std::shared_mutex> getLock() const;
};

std::ostream& operator<<(std::ostream& os, const NPC& npc);