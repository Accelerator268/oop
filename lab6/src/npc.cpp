#include "npc.h"
#include "observer.h"

NPC::NPC(NpcType t, int x, int y, const std::string& name) 
    : type(t), x(x), y(y), name(name) {}

void NPC::setPosition(int newX, int newY) {
    if (newX >= 0 && newX <= 500) x = newX;
    if (newY >= 0 && newY <= 500) y = newY;
}

void NPC::setName(const std::string& newName) {
    name = newName;
}

void NPC::subscribe(const std::shared_ptr<IFightObserver>& observer) {
    observers.push_back(observer);
}

void NPC::notifyFight(const std::shared_ptr<NPC>& defender, bool win) {
    for (auto& observer : observers) {
        observer->onFight(shared_from_this(), defender, win);
    }
}

bool NPC::isClose(const std::shared_ptr<NPC>& other, int distance) const {
    if (!other) return false;
    int dx = x - other->x;
    int dy = y - other->y;
    return (dx * dx + dy * dy) <= (distance * distance);
}

void NPC::save(std::ostream& os) const {
    os << static_cast<int>(type) << std::endl;
    os << x << std::endl;
    os << y << std::endl;
    os << name << std::endl;
    os << std::endl;  //Пустая строка для разделения записей
}

void NPC::load(std::istream& is) {
    //Читаем координату X
    std::string line;
    if (!std::getline(is, line)) {
        throw std::runtime_error("Error reading X coordinate");
    }
    
    //Удаляем пробелы
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    try {
        x = std::stoi(line);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing X coordinate: '" + line + "'");
    }
    
    //Читаем координату Y
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
    
    //Читаем имя
    if (!std::getline(is, line)) {
        throw std::runtime_error("Error reading name");
    }
    
    //Удаляем пробелы в начале и конце
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    
    name = line;
    
    //Пропускаем возможную пустую строку между записями
    std::streampos pos = is.tellg();
    if (!std::getline(is, line)) {
        return; //Конец файла
    }
    
    line.erase(0, line.find_first_not_of(" \t"));
    if (!line.empty()) {
        //Если строка не пустая, возвращаем указатель назад
        is.seekg(pos);
    }
}

std::ostream& operator<<(std::ostream& os, const NPC& npc) {
    npc.print(os);
    return os;
}