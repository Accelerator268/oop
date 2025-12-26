#include "../include/figure.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>


//Реализация операторов ввода/вывода
std::ostream& operator<<(std::ostream& os, const Figure& figure) {
    figure.print_vertices(os);
    return os;
}

std::istream& operator>>(std::istream& is, Figure& figure) {
    figure.read_vertices(is);
    return is;
}

//Вспомогательные функции
void print_figure_info(const Figure& figure) {
    auto center = figure.geometric_center();
    std::cout << "Geometric center: (" << center.first << ", " << center.second << ")\n";
    std::cout << "Area: " << figure.area() << std::endl;
}

double calculate_total_area(const std::vector<std::unique_ptr<Figure>>& figures) {
    double total = 0.0;
    for (const auto& fig : figures) {
        total += fig->area();
    }
    return total;
}

//Реализация Pentagon
// Pentagon::Pentagon(const std::vector<std::pair<double, double>>& verts) : vertices(verts) {
//     if (vertices.size() != 5) {
//         throw std::invalid_argument("Pentagon must have exactly 5 vertices");
//     }
//     if (!validate_pentagon()) {
//         throw std::invalid_argument("Invalid pentagon vertices");
//     }
// }

//Копирование и присваивание
Pentagon::Pentagon(const Pentagon& other) : vertices(other.vertices) {}
Pentagon::Pentagon(Pentagon&& other) noexcept : vertices(std::move(other.vertices)) {}

//Копирующее присваивание
Pentagon& Pentagon::operator=(const Pentagon& other) {
    if (this != &other) {
        vertices = other.vertices;
    }
    return *this;
}

//Перемещающее присваивание
Pentagon& Pentagon::operator=(Pentagon&& other) noexcept {
    if (this != &other) {
        vertices = std::move(other.vertices);
    }
    return *this;
}

bool Pentagon::validate_pentagon() const {
    //Простая проверка: все вершины должны быть различны
    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            if (vertices[i] == vertices[j]) {
                return false;
            }
        }
    }
    return true;
}

double Pentagon::area() const {
    //Используем формулу площади многоугольника (формула шнурования)
    double area = 0.0;
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t j = (i + 1) % vertices.size();
        area += vertices[i].first * vertices[j].second;
        area -= vertices[j].first * vertices[i].second;
    }
    return std::abs(area) / 2.0;
}

//Определяем геометрический центр
std::pair<double, double> Pentagon::geometric_center() const {
    double center_x = 0.0;
    double center_y = 0.0;
    
    for (const auto& vertex : vertices) {
        center_x += vertex.first;
        center_y += vertex.second;
    }
    
    return {center_x / 5.0, center_y / 5.0};
}

//Вывод координат вершин
void Pentagon::print_vertices(std::ostream& os) const {
    os << "Pentagon vertices:\n";
    for (size_t i = 0; i < vertices.size(); ++i) {
        os << "  Vertex " << i + 1 << ": (" 
           << vertices[i].first << ", " << vertices[i].second << ")\n";
    }
}

//Cxbnsdftv dthibys
void Pentagon::read_vertices(std::istream& is) {
    vertices.clear();
    std::string line;
    
    std::cout << "Enter 5 vertices for Pentagon:\n";
    for (int i = 0; i < 5; ++i) {
        double x, y;
        std::cout << "Vertex " << i + 1 << ": ";
        if (!(is >> x >> y)) {
            throw std::runtime_error("Invalid input format");
        }
        vertices.emplace_back(x, y);
    }
    
    if (!validate_pentagon()) {
        throw std::invalid_argument("Invalid pentagon vertices");
    }
}

//Копия
std::unique_ptr<Figure> Pentagon::clone() const {
    return std::make_unique<Pentagon>(*this);
}

//Перемещение
std::unique_ptr<Figure> Pentagon::move() {
    return std::make_unique<Pentagon>(std::move(*this));
}

//Hfdyj
bool Pentagon::operator==(const Figure& other) const {
    const Pentagon* pentagon = dynamic_cast<const Pentagon*>(&other);
    if (!pentagon) return false;
    
    return vertices == pentagon->vertices;
}

//Неравно
bool Pentagon::operator!=(const Figure& other) const {
    return !(*this == other);
}

//Реализация Hexagon
// Hexagon::Hexagon(const std::vector<std::pair<double, double>>& verts) : vertices(verts) {
//     if (vertices.size() != 6) {
//         throw std::invalid_argument("Hexagon must have exactly 6 vertices");
//     }
//     if (!validate_hexagon()) {
//         throw std::invalid_argument("Invalid hexagon vertices");
//     }
// }

Hexagon::Hexagon(const Hexagon& other) : vertices(other.vertices) {}

Hexagon::Hexagon(Hexagon&& other) noexcept : vertices(std::move(other.vertices)) {}

Hexagon& Hexagon::operator=(const Hexagon& other) {
    if (this != &other) {
        vertices = other.vertices;
    }
    return *this;
}

Hexagon& Hexagon::operator=(Hexagon&& other) noexcept {
    if (this != &other) {
        vertices = std::move(other.vertices);
    }
    return *this;
}

bool Hexagon::validate_hexagon() const {
    //Простая проверка: все вершины должны быть различны
    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            if (vertices[i] == vertices[j]) {
                return false;
            }
        }
    }
    return true;
}

double Hexagon::area() const {
    //Формула площади многоугольника
    double area = 0.0;
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t j = (i + 1) % vertices.size();
        area += vertices[i].first * vertices[j].second;
        area -= vertices[j].first * vertices[i].second;
    }
    return std::abs(area) / 2.0;
}

std::pair<double, double> Hexagon::geometric_center() const {
    double center_x = 0.0;
    double center_y = 0.0;
    
    for (const auto& vertex : vertices) {
        center_x += vertex.first;
        center_y += vertex.second;
    }
    
    return {center_x / 6.0, center_y / 6.0};
}

void Hexagon::print_vertices(std::ostream& os) const {
    os << "Hexagon vertices:\n";
    for (size_t i = 0; i < vertices.size(); ++i) {
        os << "  Vertex " << i + 1 << ": (" 
           << vertices[i].first << ", " << vertices[i].second << ")\n";
    }
}

void Hexagon::read_vertices(std::istream& is) {
    vertices.clear();
    std::string line;
    
    std::cout << "Enter 6 vertices for Hexagon:\n";
    for (int i = 0; i < 6; ++i) {
        double x, y;
        std::cout << "Vertex " << i + 1 << ": ";
        if (!(is >> x >> y)) {
            throw std::runtime_error("Invalid input format");
        }
        vertices.emplace_back(x, y);
    }
    
    if (!validate_hexagon()) {
        throw std::invalid_argument("Invalid hexagon vertices");
    }
}

std::unique_ptr<Figure> Hexagon::clone() const {
    return std::make_unique<Hexagon>(*this);
}

std::unique_ptr<Figure> Hexagon::move() {
    return std::make_unique<Hexagon>(std::move(*this));
}

bool Hexagon::operator==(const Figure& other) const {
    const Hexagon* hexagon = dynamic_cast<const Hexagon*>(&other);
    if (!hexagon) return false;
    
    return vertices == hexagon->vertices;
}

bool Hexagon::operator!=(const Figure& other) const {
    return !(*this == other);
}

//Реализация Octagon
// Octagon::Octagon(const std::vector<std::pair<double, double>>& verts) : vertices(verts) {
//     if (vertices.size() != 8) {
//         throw std::invalid_argument("Octagon must have exactly 8 vertices");
//     }
//     if (!validate_octagon()) {
//         throw std::invalid_argument("Invalid octagon vertices");
//     }
// }

Octagon::Octagon(const Octagon& other) : vertices(other.vertices) {}

Octagon::Octagon(Octagon&& other) noexcept : vertices(std::move(other.vertices)) {}

Octagon& Octagon::operator=(const Octagon& other) {
    if (this != &other) {
        vertices = other.vertices;
    }
    return *this;
}

Octagon& Octagon::operator=(Octagon&& other) noexcept {
    if (this != &other) {
        vertices = std::move(other.vertices);
    }
    return *this;
}

bool Octagon::validate_octagon() const {
    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            if (vertices[i] == vertices[j]) {
                return false;
            }
        }
    }
    return true;
}

double Octagon::area() const {
    double area = 0.0;
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t j = (i + 1) % vertices.size();
        area += vertices[i].first * vertices[j].second;
        area -= vertices[j].first * vertices[i].second;
    }
    return std::abs(area) / 2.0;
}

std::pair<double, double> Octagon::geometric_center() const {
    double center_x = 0.0;
    double center_y = 0.0;
    
    for (const auto& vertex : vertices) {
        center_x += vertex.first;
        center_y += vertex.second;
    }
    
    return {center_x / 8.0, center_y / 8.0};
}

void Octagon::print_vertices(std::ostream& os) const {
    os << "Octagon vertices:\n";
    for (size_t i = 0; i < vertices.size(); ++i) {
        os << "  Vertex " << i + 1 << ": (" 
           << vertices[i].first << ", " << vertices[i].second << ")\n";
    }
}

void Octagon::read_vertices(std::istream& is) {
    vertices.clear();
    std::string line;
    
    std::cout << "Enter 8 vertices for Octagon:\n";
    for (int i = 0; i < 8; ++i) {
        double x, y;
        std::cout << "Vertex " << i + 1 << ": ";
        if (!(is >> x >> y)) {
            throw std::runtime_error("Invalid input format");
        }
        vertices.emplace_back(x, y);
    }
    
    if (!validate_octagon()) {
        throw std::invalid_argument("Invalid octagon vertices");
    }
}

std::unique_ptr<Figure> Octagon::clone() const {
    return std::make_unique<Octagon>(*this);
}

std::unique_ptr<Figure> Octagon::move() {
    return std::make_unique<Octagon>(std::move(*this));
}

bool Octagon::operator==(const Figure& other) const {
    const Octagon* octagon = dynamic_cast<const Octagon*>(&other);
    if (!octagon) return false;
    
    return vertices == octagon->vertices;
}

bool Octagon::operator!=(const Figure& other) const {
    return !(*this == other);
}

//Фабрика фигур
std::unique_ptr<Figure> create_figure(const std::string& type) {
    if (type == "pentagon" || type == "5") {
        return std::make_unique<Pentagon>();
    } else if (type == "hexagon" || type == "6") {
        return std::make_unique<Hexagon>();
    } else if (type == "octagon" || type == "8") {
        return std::make_unique<Octagon>();
    } else {
        throw std::invalid_argument("Unknown figure type: " + type);
    }
}