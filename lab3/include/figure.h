#ifndef FIGURE_H
#define FIGURE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

//Базовый класс для всех фигур
class Figure {
public:
    virtual ~Figure() = default;
    
    //Виртуальные методы
    virtual double area() const = 0;
    virtual std::pair<double, double> geometric_center() const = 0;
    virtual void print_vertices(std::ostream& os) const = 0;
    virtual void read_vertices(std::istream& is) = 0;
    
    //Виртуальные методы для копирования и перемещения
    virtual std::unique_ptr<Figure> clone() const = 0;
    virtual std::unique_ptr<Figure> move() = 0;
    
    //Операторы сравнения
    virtual bool operator==(const Figure& other) const = 0;
    virtual bool operator!=(const Figure& other) const = 0;
    
    //Приведение к площади double
    explicit operator double() const {
        return area();
    }
    
    //Дружественные операторы ввода/вывода
    friend std::ostream& operator<<(std::ostream& os, const Figure& figure);
    friend std::istream& operator>>(std::istream& is, Figure& figure);
};

//Класс 5-угольника (пентагон)
class Pentagon : public Figure {
private:
    std::vector<std::pair<double, double>> vertices;
    
    //Проверка, что фигура является правильным пентагоном
    bool validate_pentagon() const;
    
public:
    Pentagon() = default;
    Pentagon(const std::vector<std::pair<double, double>>& verts);
    
    //Копирование и перемещение
    Pentagon(const Pentagon& other);
    Pentagon(Pentagon&& other) noexcept;
    Pentagon& operator=(const Pentagon& other);
    Pentagon& operator=(Pentagon&& other) noexcept;
    
    //Реализация виртуальных методов
    double area() const override;
    std::pair<double, double> geometric_center() const override;
    void print_vertices(std::ostream& os) const override;
    void read_vertices(std::istream& is) override;
    
    //Клонирование
    std::unique_ptr<Figure> clone() const override;
    std::unique_ptr<Figure> move() override;
    
    //Операторы сравнения
    bool operator==(const Figure& other) const override;
    bool operator!=(const Figure& other) const override;
    
    //Получение вершин
    const std::vector<std::pair<double, double>>& get_vertices() const { return vertices; }
};

//Класс 6-угольника (гексагон)
class Hexagon : public Figure {
private:
    std::vector<std::pair<double, double>> vertices;
    
    //Проверка, что фигура является правильным гексагоном
    bool validate_hexagon() const;
    
public:
    Hexagon() = default;
    Hexagon(const std::vector<std::pair<double, double>>& verts);
    
    //Копирование и перемещение
    Hexagon(const Hexagon& other);
    Hexagon(Hexagon&& other) noexcept;
    Hexagon& operator=(const Hexagon& other);
    Hexagon& operator=(Hexagon&& other) noexcept;
    
    //Реализация виртуальных методов
    double area() const override;
    std::pair<double, double> geometric_center() const override;
    void print_vertices(std::ostream& os) const override;
    void read_vertices(std::istream& is) override;
    
    //Клонирование
    std::unique_ptr<Figure> clone() const override;
    std::unique_ptr<Figure> move() override;
    
    //Операторы сравнения
    bool operator==(const Figure& other) const override;
    bool operator!=(const Figure& other) const override;
    
    //Получение вершин
    const std::vector<std::pair<double, double>>& get_vertices() const { return vertices; }
};

//Класс 8-угольника (октагон)
class Octagon : public Figure {
private:
    std::vector<std::pair<double, double>> vertices;
    
    //Проверка, что фигура является правильным октагоном
    bool validate_octagon() const;
    
public:
    Octagon() = default;
    Octagon(const std::vector<std::pair<double, double>>& verts);
    
    //Копирование и перемещение
    Octagon(const Octagon& other);
    Octagon(Octagon&& other) noexcept;
    Octagon& operator=(const Octagon& other);
    Octagon& operator=(Octagon&& other) noexcept;
    
    //Реализация виртуальных методов
    double area() const override;
    std::pair<double, double> geometric_center() const override;
    void print_vertices(std::ostream& os) const override;
    void read_vertices(std::istream& is) override;
    
    //Клонирование
    std::unique_ptr<Figure> clone() const override;
    std::unique_ptr<Figure> move() override;
    
    //Операторы сравнения
    bool operator==(const Figure& other) const override;
    bool operator!=(const Figure& other) const override;
    
    //Получение вершин
    const std::vector<std::pair<double, double>>& get_vertices() const { return vertices; }
};

//Фабрика для создания фигур
std::unique_ptr<Figure> create_figure(const std::string& type);

//Вспомогательные функции
void print_figure_info(const Figure& figure);
double calculate_total_area(const std::vector<std::unique_ptr<Figure>>& figures);

#endif //FIGURE_H