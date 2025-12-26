#include "npc.h"
#include "observer.h"
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dice(1, 6);

NPC::NPC(NpcType t, int x, int y, const std::string& name) 
    : type(t), x(x), y(y), name(name), alive(true) {}

NpcType NPC::getType() const {
    std::shared_lock lock(mutex);
    return type;
}

int NPC::getX() const {
    std::shared_lock lock(mutex);
    return x;
}

int NPC::getY() const {
    std::shared_lock lock(mutex);
    return y;
}

const std::string& NPC::getName() const {
    std::shared_lock lock(mutex);
    return name;
}

bool NPC::isAlive() const {
    std::shared_lock lock(mutex);
    return alive;
}

void NPC::setPosition(int newX, int newY) {
    std::unique_lock lock(mutex);
    if (newX >= 0 && newX <= 500) x = newX;
    if (newY >= 0 && newY <= 500) y = newY;
}

void NPC::setName(const std::string& newName) {
    std::unique_lock lock(mutex);
    name = newName;
}

void NPC::setAlive(bool isAlive) {
    std::unique_lock lock(mutex);
    alive = isAlive;
}

void NPC::subscribe(const std::shared_ptr<IFightObserver>& observer) {
    std::unique_lock lock(mutex);
    observers.push_back(observer);
}

void NPC::notifyFight(const std::shared_ptr<NPC>& defender, bool win) {
    std::vector<std::shared_ptr<IFightObserver>> observersCopy;
    {
        std::shared_lock lock(mutex);
        observersCopy = observers;
    }
    
    for (auto& observer : observersCopy) {
        observer->onFight(shared_from_this(), defender, win);
    }
}

bool NPC::isClose(const std::shared_ptr<NPC>& other, int distance) const {
    if (!other) return false;
    
    // Используем shared_lock для чтения позиций
    int otherX, otherY, thisX, thisY;
    {
        std::shared_lock lock1(mutex);
        thisX = x;
        thisY = y;
    }
    {
        auto otherLock = other->getLock();
        otherX = other->x;
        otherY = other->y;
    }
    
    int dx = thisX - otherX;
    int dy = thisY - otherY;
    return (dx * dx + dy * dy) <= (distance * distance);
}

int NPC::rollAttack() const {
    return dice(gen);
}

int NPC::rollDefense() const {
    return dice(gen);
}

std::unique_lock<std::shared_mutex> NPC::getLock() const {
    return std::unique_lock<std::shared_mutex>(const_cast<std::shared_mutex&>(mutex));
}

void NPC::save(std::ostream& os) const {
    std::shared_lock lock(mutex);
    os << static_cast<int>(type) << std::endl;
    os << x << std::endl;
    os << y << std::endl;
    os << name << std::endl;
    os << (alive ? 1 : 0) << std::endl;  // Сохраняем состояние alive
    os << std::endl;  // Пустая строка для разделения записей
}

void NPC::load(std::istream& is) {
    std::unique_lock lock(mutex);
    
    // Читаем координату X
    std::string line;
    if (!std::getline(is, line)) {
        throw std::runtime_error("Error reading X coordinate");
    }
    
    // Удаляем пробелы
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    try {
        x = std::stoi(line);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing X coordinate: '" + line + "'");
    }
    
    // Читаем координату Y
    if (!std::getline(is, line)) {
        throw std::runtime_error("Error reading Y coordinate");
    }
    
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    try {
        y = std::stoi(line);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing Y coordinate: '" + line + "'");
    }
    
    // Читаем имя
    if (!std::getline(is, line)) {
        throw std::runtime_error("Error reading name");
    }
    
    // Удаляем пробелы в начале и конце
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    name = line;
    
    // Читаем состояние alive
    if (!std::getline(is, line)) {
        throw std::runtime_error("Error reading alive state");
    }
    
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    try {
        alive = (std::stoi(line) != 0);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing alive state: '" + line + "'");
    }
    
    // Пропускаем возможную пустую строку между записями
    std::streampos pos = is.tellg();
    if (!std::getline(is, line)) {
        return; // Конец файла
    }
    
    line.erase(0, line.find_first_not_of(" \t"));
    if (!line.empty()) {
        // Если строка не пустая, возвращаем указатель назад
        is.seekg(pos);
    }
}

std::ostream& operator<<(std::ostream& os, const NPC& npc) {
    npc.print(os);
    return os;
}