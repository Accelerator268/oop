#ifndef FIGURE_H
#define FIGURE_H

#include <iostream>
#include <memory>
#include <concepts>
#include <cmath>
#include <vector>
#include <algorithm>
#include <initializer_list>
#include <type_traits>

//Концепт для скалярных типов
template<typename T>
concept Scalar = std::is_arithmetic_v<T>;

//Шаблонный класс точки
template<Scalar T>
class Point {
private:
    T x_, y_;
public:
    Point() : x_(0), y_(0) {}
    Point(T x, T y) : x_(x), y_(y) {}
    
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;
    Point(Point&&) = default;
    Point& operator=(Point&&) = default;
    
    T x() const { return x_; }
    T y() const { return y_; }
    
    void set_x(T x) { x_ = x; }
    void set_y(T y) { y_ = y; }
    
    bool operator==(const Point<T>& other) const {
        if constexpr (std::is_integral_v<T>) {
            return x_ == other.x_ && y_ == other.y_;
        } else {
            return std::abs(x_ - other.x_) < std::numeric_limits<T>::epsilon() * 10 &&
                   std::abs(y_ - other.y_) < std::numeric_limits<T>::epsilon() * 10;
        }
    }
    
    bool operator!=(const Point<T>& other) const {
        return !(*this == other);
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Point<T>& p) {
        return os << "(" << p.x_ << ", " << p.y_ << ")";
    }
    
    friend std::istream& operator>>(std::istream& is, Point<T>& p) {
        return is >> p.x_ >> p.y_;
    }
};

//Базовый шаблонный класс фигуры
template<Scalar T>
class Figure {
protected:
    std::vector<Point<T>> vertices;  //Храним точки напрямую, а не через указатели
    
public:
    virtual ~Figure() = default;
    
    //Виртуальные методы
    virtual double area() const = 0;
    virtual Point<T> geometric_center() const = 0;
    virtual void print_vertices(std::ostream& os) const = 0;
    virtual void read_vertices(std::istream& is) = 0;
    
    //Виртуальные методы для копирования
    virtual std::shared_ptr<Figure<T>> clone() const = 0;
    
    //Операторы сравнения
    virtual bool operator==(const Figure<T>& other) const = 0;
    virtual bool operator!=(const Figure<T>& other) const = 0;
    
    //Приведение к площади double
    explicit operator double() const {
        return area();
    }
    
    //Получение вершин
    const std::vector<Point<T>>& get_vertices() const {
        return vertices;
    }
    
    size_t vertex_count() const {
        return vertices.size();
    }
};

//Шаблонный класс квадрата
template<Scalar T>
class Square : public Figure<T> {
public:
    Square() = default;
    Square(const std::vector<Point<T>>& verts);
    
    //Реализация виртуальных методов
    double area() const override;
    Point<T> geometric_center() const override;
    void print_vertices(std::ostream& os) const override;
    void read_vertices(std::istream& is) override;
    
    //Клонирование
    std::shared_ptr<Figure<T>> clone() const override;
    
    //Операторы сравнения
    bool operator==(const Figure<T>& other) const override;
    bool operator!=(const Figure<T>& other) const override;
    
private:
    bool validate_square() const;
};

//Шаблонный класс прямоугольника
template<Scalar T>
class Rectangle : public Figure<T> {
public:
    Rectangle() = default;
    Rectangle(const std::vector<Point<T>>& verts);
    
    //Реализация виртуальных методов
    double area() const override;
    Point<T> geometric_center() const override;
    void print_vertices(std::ostream& os) const override;
    void read_vertices(std::istream& is) override;
    
    //Клонирование
    std::shared_ptr<Figure<T>> clone() const override;
    
    //Операторы сравнения
    bool operator==(const Figure<T>& other) const override;
    bool operator!=(const Figure<T>& other) const override;
    
private:
    bool validate_rectangle() const;
};

//Шаблонный класс трапеции
template<Scalar T>
class Trapezoid : public Figure<T> {
public:
    Trapezoid() = default;
    Trapezoid(const std::vector<Point<T>>& verts);
    
    //Реализация виртуальных методов
    double area() const override;
    Point<T> geometric_center() const override;
    void print_vertices(std::ostream& os) const override;
    void read_vertices(std::istream& is) override;
    
    //Клонирование
    std::shared_ptr<Figure<T>> clone() const override;
    
    //Операторы сравнения
    bool operator==(const Figure<T>& other) const override;
    bool operator!=(const Figure<T>& other) const override;
    
private:
    bool validate_trapezoid() const;
};

//Шаблонный динамический массив
template<typename T>
class Array {
private:
    T* data;
    size_t capacity_;
    size_t size_;
    
    void resize(size_t new_capacity);
    
public:
    Array() : data(nullptr), capacity_(0), size_(0) {}
    explicit Array(size_t initial_capacity);
    Array(std::initializer_list<T> init);
    ~Array();
    
    //Копирование и перемещение
    Array(const Array& other);
    Array(Array&& other) noexcept;
    Array& operator=(const Array& other);
    Array& operator=(Array&& other) noexcept;
    
    //Доступ к элементам
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    
    //Модификация массива
    void push_back(const T& value);
    void push_back(T&& value);
    void erase(size_t index);
    void clear();
    
    //Информация о массиве
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    
    //Итераторы
    T* begin() { return data; }
    T* end() { return data + size_; }
    const T* begin() const { return data; }
    const T* end() const { return data + size_; }
};

//Операторы ввода/вывода для Figure
template<Scalar T>
std::ostream& operator<<(std::ostream& os, const Figure<T>& figure);

template<Scalar T>
std::istream& operator>>(std::istream& is, Figure<T>& figure);

//Фабрика для создания фигур
template<Scalar T>
std::shared_ptr<Figure<T>> create_figure(const std::string& type);

//Вспомогательные функции
template<Scalar T>
void print_figure_info(const Figure<T>& figure);

template<Scalar T>
double calculate_total_area(const Array<std::shared_ptr<Figure<T>>>& figures);

#endif //FIGURE_H