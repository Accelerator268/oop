#include "../include/figure.h"
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <numeric>

//Операторы ввода/вывода для Figure
template<Scalar T>
std::ostream& operator<<(std::ostream& os, const Figure<T>& figure) {
    figure.print_vertices(os);
    return os;
}

template<Scalar T>
std::istream& operator>>(std::istream& is, Figure<T>& figure) {
    figure.read_vertices(is);
    return is;
}

//Реализация методов Square
template<Scalar T>
Square<T>::Square(const std::vector<Point<T>>& verts) {
    if (verts.size() != 4) {
        throw std::invalid_argument("Square must have exactly 4 vertices");
    }
    this->vertices = verts;
    if (!validate_square()) {
        throw std::invalid_argument("Invalid square vertices");
    }
}

template<Scalar T>
bool Square<T>::validate_square() const {
    if (this->vertices.size() != 4) return false;
    
    //Вычисляем квадраты длин сторон
    auto dist_sq = [](const Point<T>& p1, const Point<T>& p2) {
        T dx = p1.x() - p2.x();
        T dy = p1.y() - p2.y();
        return dx * dx + dy * dy;
    };
    
    T side1_sq = dist_sq(this->vertices[0], this->vertices[1]);
    T side2_sq = dist_sq(this->vertices[1], this->vertices[2]);
    T side3_sq = dist_sq(this->vertices[2], this->vertices[3]);
    T side4_sq = dist_sq(this->vertices[3], this->vertices[0]);
    
    T eps = static_cast<T>(1e-10);
    if constexpr (std::is_floating_point_v<T>) {
        //Проверяем равенство всех сторон для квадрата
        if (std::abs(side1_sq - side2_sq) > eps ||
            std::abs(side2_sq - side3_sq) > eps ||
            std::abs(side3_sq - side4_sq) > eps) {
            return false;
        }
        
        //Проверяем прямой угол через скалярное произведение
        auto vector = [](const Point<T>& from, const Point<T>& to) {
            return std::make_pair(to.x() - from.x(), to.y() - from.y());
        };
        
        auto [v1x, v1y] = vector(this->vertices[0], this->vertices[1]);
        auto [v2x, v2y] = vector(this->vertices[1], this->vertices[2]);
        
        T dot_product = v1x * v2x + v1y * v2y;
        return std::abs(dot_product) < eps;
    } else {
        //Для целых чисел
        return (side1_sq == side2_sq) && (side2_sq == side3_sq) && (side3_sq == side4_sq);
    }
}

template<Scalar T>
double Square<T>::area() const {
    if (this->vertices.size() != 4) return 0.0;
    
    //Длина стороны
    T dx = this->vertices[0].x() - this->vertices[1].x();
    T dy = this->vertices[0].y() - this->vertices[1].y();
    T side_sq = dx * dx + dy * dy;
    
    if constexpr (std::is_floating_point_v<T>) {
        return static_cast<double>(std::sqrt(side_sq) * std::sqrt(side_sq));
    } else {
        return static_cast<double>(side_sq);
    }
}

template<Scalar T>
Point<T> Square<T>::geometric_center() const {
    if (this->vertices.empty()) return Point<T>(0, 0);
    
    T center_x = 0, center_y = 0;
    for (const auto& vertex : this->vertices) {
        center_x += vertex.x();
        center_y += vertex.y();
    }
    
    return Point<T>(center_x / 4, center_y / 4);
}

template<Scalar T>
void Square<T>::print_vertices(std::ostream& os) const {
    os << "Square vertices:\n";
    for (size_t i = 0; i < this->vertices.size(); ++i) {
        os << "  Vertex " << i + 1 << ": " << this->vertices[i] << "\n";
    }
}

template<Scalar T>
void Square<T>::read_vertices(std::istream& is) {
    this->vertices.clear();
    std::cout << "Enter 4 vertices for Square (clockwise or counter-clockwise):\n";
    for (int i = 0; i < 4; ++i) {
        T x, y;
        std::cout << "Vertex " << i + 1 << " (x y): ";
        if (!(is >> x >> y)) {
            throw std::runtime_error("Invalid input format");
        }
        this->vertices.emplace_back(x, y);
    }
    
    if (!validate_square()) {
        throw std::invalid_argument("Invalid square vertices");
    }
}

template<Scalar T>
std::shared_ptr<Figure<T>> Square<T>::clone() const {
    return std::make_shared<Square<T>>(this->vertices);
}

template<Scalar T>
bool Square<T>::operator==(const Figure<T>& other) const {
    const Square<T>* square = dynamic_cast<const Square<T>*>(&other);
    if (!square) return false;
    
    if (this->vertices.size() != square->vertices.size()) return false;
    
    for (size_t i = 0; i < this->vertices.size(); ++i) {
        if (this->vertices[i] != square->vertices[i]) return false;
    }
    
    return true;
}

template<Scalar T>
bool Square<T>::operator!=(const Figure<T>& other) const {
    return !(*this == other);
}

//Реализация методов Rectangle
template<Scalar T>
Rectangle<T>::Rectangle(const std::vector<Point<T>>& verts) {
    if (verts.size() != 4) {
        throw std::invalid_argument("Rectangle must have exactly 4 vertices");
    }
    this->vertices = verts;
    if (!validate_rectangle()) {
        throw std::invalid_argument("Invalid rectangle vertices");
    }
}

template<Scalar T>
bool Rectangle<T>::validate_rectangle() const {
    if (this->vertices.size() != 4) return false;
    
    auto dist_sq = [](const Point<T>& p1, const Point<T>& p2) {
        T dx = p1.x() - p2.x();
        T dy = p1.y() - p2.y();
        return dx * dx + dy * dy;
    };
    
    //Длины противоположных сторон должны быть равны
    T side1_sq = dist_sq(this->vertices[0], this->vertices[1]);
    T side3_sq = dist_sq(this->vertices[2], this->vertices[3]);
    
    T side2_sq = dist_sq(this->vertices[1], this->vertices[2]);
    T side4_sq = dist_sq(this->vertices[3], this->vertices[0]);
    
    T eps = static_cast<T>(1e-10);
    
    if constexpr (std::is_floating_point_v<T>) {
        if (std::abs(side1_sq - side3_sq) > eps || std::abs(side2_sq - side4_sq) > eps) {
            return false;
        }
        
        //Проверяем, что все углы 90 градусов
        auto vector = [](const Point<T>& from, const Point<T>& to) {
            return std::make_pair(to.x() - from.x(), to.y() - from.y());
        };
        
        //Проверяем прямой угол в первой вершине
        auto [v1x, v1y] = vector(this->vertices[0], this->vertices[1]);
        auto [v2x, v2y] = vector(this->vertices[0], this->vertices[3]);
        
        T dot_product = v1x * v2x + v1y * v2y;
        return std::abs(dot_product) < eps;
    } else {
        return (side1_sq == side3_sq) && (side2_sq == side4_sq);
    }
}

template<Scalar T>
double Rectangle<T>::area() const {
    if (this->vertices.size() != 4) return 0.0;
    
    auto dist = [](const Point<T>& p1, const Point<T>& p2) {
        T dx = p1.x() - p2.x();
        T dy = p1.y() - p2.y();
        return std::sqrt(dx * dx + dy * dy);
    };
    
    if constexpr (std::is_floating_point_v<T>) {
        T width = dist(this->vertices[0], this->vertices[1]);
        T height = dist(this->vertices[1], this->vertices[2]);
        return static_cast<double>(width * height);
    } else {
        //Для целых чисел
        T width_sq = (this->vertices[0].x() - this->vertices[1].x()) * 
                    (this->vertices[0].x() - this->vertices[1].x()) +
                    (this->vertices[0].y() - this->vertices[1].y()) * 
                    (this->vertices[0].y() - this->vertices[1].y());
        
        T height_sq = (this->vertices[1].x() - this->vertices[2].x()) * 
                     (this->vertices[1].x() - this->vertices[2].x()) +
                     (this->vertices[1].y() - this->vertices[2].y()) * 
                     (this->vertices[1].y() - this->vertices[2].y());
        
        return static_cast<double>(std::sqrt(width_sq * height_sq));
    }
}

template<Scalar T>
Point<T> Rectangle<T>::geometric_center() const {
    if (this->vertices.empty()) return Point<T>(0, 0);
    
    T center_x = 0, center_y = 0;
    for (const auto& vertex : this->vertices) {
        center_x += vertex.x();
        center_y += vertex.y();
    }
    
    return Point<T>(center_x / 4, center_y / 4);
}

template<Scalar T>
void Rectangle<T>::print_vertices(std::ostream& os) const {
    os << "Rectangle vertices:\n";
    for (size_t i = 0; i < this->vertices.size(); ++i) {
        os << "  Vertex " << i + 1 << ": " << this->vertices[i] << "\n";
    }
}

template<Scalar T>
void Rectangle<T>::read_vertices(std::istream& is) {
    this->vertices.clear();
    std::cout << "Enter 4 vertices for Rectangle (clockwise or counter-clockwise):\n";
    for (int i = 0; i < 4; ++i) {
        T x, y;
        std::cout << "Vertex " << i + 1 << " (x y): ";
        if (!(is >> x >> y)) {
            throw std::runtime_error("Invalid input format");
        }
        this->vertices.emplace_back(x, y);
    }
    
    if (!validate_rectangle()) {
        throw std::invalid_argument("Invalid rectangle vertices");
    }
}

template<Scalar T>
std::shared_ptr<Figure<T>> Rectangle<T>::clone() const {
    return std::make_shared<Rectangle<T>>(this->vertices);
}

template<Scalar T>
bool Rectangle<T>::operator==(const Figure<T>& other) const {
    const Rectangle<T>* rect = dynamic_cast<const Rectangle<T>*>(&other);
    if (!rect) return false;
    
    if (this->vertices.size() != rect->vertices.size()) return false;
    
    for (size_t i = 0; i < this->vertices.size(); ++i) {
        if (this->vertices[i] != rect->vertices[i]) return false;
    }
    
    return true;
}

template<Scalar T>
bool Rectangle<T>::operator!=(const Figure<T>& other) const {
    return !(*this == other);
}

//Реализация методов Trapezoid
template<Scalar T>
Trapezoid<T>::Trapezoid(const std::vector<Point<T>>& verts) {
    if (verts.size() != 4) {
        throw std::invalid_argument("Trapezoid must have exactly 4 vertices");
    }
    this->vertices = verts;
    if (!validate_trapezoid()) {
        throw std::invalid_argument("Invalid trapezoid vertices");
    }
}

template<Scalar T>
bool Trapezoid<T>::validate_trapezoid() const {
    if (this->vertices.size() != 4) return false;
    
    //Векторное произведение для проверки параллельности
    auto cross_product = [](const Point<T>& a, const Point<T>& b, const Point<T>& c, const Point<T>& d) {
        T ab_x = b.x() - a.x();
        T ab_y = b.y() - a.y();
        T cd_x = d.x() - c.x();
        T cd_y = d.y() - c.y();
        return ab_x * cd_y - ab_y * cd_x;
    };
    
    T eps = static_cast<T>(1e-10);
    
    //Проверяем параллельность противоположных сторон
    T cp1 = cross_product(this->vertices[0], this->vertices[1], 
                         this->vertices[2], this->vertices[3]);
    T cp2 = cross_product(this->vertices[1], this->vertices[2], 
                         this->vertices[3], this->vertices[0]);
    
    if constexpr (std::is_floating_point_v<T>) {
        return std::abs(cp1) < eps || std::abs(cp2) < eps;
    } else {
        return cp1 == 0 || cp2 == 0;
    }
}

template<Scalar T>
double Trapezoid<T>::area() const {
    if (this->vertices.size() != 4) return 0.0;
    
    //Используем формулу площади Гаусса
    double sum = 0.0;
    for (int i = 0; i < 4; ++i) {
        int j = (i + 1) % 4;
        sum += this->vertices[i].x() * this->vertices[j].y() -
               this->vertices[j].x() * this->vertices[i].y();
    }
    
    return std::abs(sum) / 2.0;
}

template<Scalar T>
Point<T> Trapezoid<T>::geometric_center() const {
    if (this->vertices.empty()) return Point<T>(0, 0);
    
    T center_x = 0, center_y = 0;
    for (const auto& vertex : this->vertices) {
        center_x += vertex.x();
        center_y += vertex.y();
    }
    
    return Point<T>(center_x / 4, center_y / 4);
}

template<Scalar T>
void Trapezoid<T>::print_vertices(std::ostream& os) const {
    os << "Trapezoid vertices:\n";
    for (size_t i = 0; i < this->vertices.size(); ++i) {
        os << "  Vertex " << i + 1 << ": " << this->vertices[i] << "\n";
    }
}

template<Scalar T>
void Trapezoid<T>::read_vertices(std::istream& is) {
    this->vertices.clear();
    std::cout << "Enter 4 vertices for Trapezoid (in order, with parallel sides):\n";
    for (int i = 0; i < 4; ++i) {
        T x, y;
        std::cout << "Vertex " << i + 1 << " (x y): ";
        if (!(is >> x >> y)) {
            throw std::runtime_error("Invalid input format");
        }
        this->vertices.emplace_back(x, y);
    }
    
    if (!validate_trapezoid()) {
        throw std::invalid_argument("Invalid trapezoid vertices");
    }
}

template<Scalar T>
std::shared_ptr<Figure<T>> Trapezoid<T>::clone() const {
    return std::make_shared<Trapezoid<T>>(this->vertices);
}

template<Scalar T>
bool Trapezoid<T>::operator==(const Figure<T>& other) const {
    const Trapezoid<T>* trap = dynamic_cast<const Trapezoid<T>*>(&other);
    if (!trap) return false;
    
    if (this->vertices.size() != trap->vertices.size()) return false;
    
    for (size_t i = 0; i < this->vertices.size(); ++i) {
        if (this->vertices[i] != trap->vertices[i]) return false;
    }
    
    return true;
}

template<Scalar T>
bool Trapezoid<T>::operator!=(const Figure<T>& other) const {
    return !(*this == other);
}

//Реализация фабрики фигур
template<Scalar T>
std::shared_ptr<Figure<T>> create_figure(const std::string& type) {
    if (type == "square" || type == "4") {
        return std::make_shared<Square<T>>();
    } else if (type == "rectangle" || type == "rect") {
        return std::make_shared<Rectangle<T>>();
    } else if (type == "trapezoid" || type == "trap") {
        return std::make_shared<Trapezoid<T>>();
    } else {
        throw std::invalid_argument("Unknown figure type: " + type);
    }
}

//Вспомогательная функция для печати информации о фигуре
template<Scalar T>
void print_figure_info(const Figure<T>& figure) {
    auto center = figure.geometric_center();
    std::cout << "Geometric center: " << center << "\n";
    std::cout << "Area: " << figure.area() << std::endl;
}

//Реализация шаблонного массива
template<typename T>
Array<T>::Array(size_t initial_capacity) 
    : data(new T[initial_capacity]), 
      capacity_(initial_capacity), 
      size_(0) {}

template<typename T>
Array<T>::Array(std::initializer_list<T> init)
    : data(new T[init.size()]),
      capacity_(init.size()),
      size_(init.size()) {
    std::copy(init.begin(), init.end(), data);
}

template<typename T>
Array<T>::~Array() {
    delete[] data;
}

template<typename T>
void Array<T>::resize(size_t new_capacity) {
    if (new_capacity <= capacity_) return;
    
    T* new_data = new T[new_capacity];
    
    //Используем перемещение при увеличении емкости
    for (size_t i = 0; i < size_; ++i) {
        new_data[i] = std::move(data[i]);
    }
    
    delete[] data;
    data = new_data;
    capacity_ = new_capacity;
}

template<typename T>
Array<T>::Array(const Array& other) 
    : data(new T[other.capacity_]), 
      capacity_(other.capacity_), 
      size_(other.size_) {
    for (size_t i = 0; i < size_; ++i) {
        data[i] = other.data[i];
    }
}

template<typename T>
Array<T>::Array(Array&& other) noexcept 
    : data(other.data), 
      capacity_(other.capacity_), 
      size_(other.size_) {
    other.data = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
}

template<typename T>
Array<T>& Array<T>::operator=(const Array& other) {
    if (this != &other) {
        delete[] data;
        capacity_ = other.capacity_;
        size_ = other.size_;
        data = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(Array&& other) noexcept {
    if (this != &other) {
        delete[] data;
        data = other.data;
        capacity_ = other.capacity_;
        size_ = other.size_;
        other.data = nullptr;
        other.capacity_ = 0;
        other.size_ = 0;
    }
    return *this;
}

template<typename T>
T& Array<T>::operator[](size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Array index out of bounds");
    }
    return data[index];
}

template<typename T>
const T& Array<T>::operator[](size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Array index out of bounds");
    }
    return data[index];
}

template<typename T>
void Array<T>::push_back(const T& value) {
    if (size_ >= capacity_) {
        resize(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    data[size_++] = value;
}

template<typename T>
void Array<T>::push_back(T&& value) {
    if (size_ >= capacity_) {
        resize(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    data[size_++] = std::move(value);
}

template<typename T>
void Array<T>::erase(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Array index out of bounds");
    }
    
    for (size_t i = index; i < size_ - 1; ++i) {
        data[i] = std::move(data[i + 1]);
    }
    --size_;
}

template<typename T>
void Array<T>::clear() {
    size_ = 0;
}

//Вспомогательная функция для вычисления общей площади
template<Scalar T>
double calculate_total_area(const Array<std::shared_ptr<Figure<T>>>& figures) {
    double total = 0.0;
    for (size_t i = 0; i < figures.size(); ++i) {
        total += figures[i]->area();
    }
    return total;
}

//Явная инстанциация для часто используемых типов
template class Point<int>;
template class Point<double>;
template class Point<float>;

template class Figure<int>;
template class Figure<double>;
template class Figure<float>;

template class Square<int>;
template class Square<double>;
template class Square<float>;

template class Rectangle<int>;
template class Rectangle<double>;
template class Rectangle<float>;

template class Trapezoid<int>;
template class Trapezoid<double>;
template class Trapezoid<float>;

template class Array<std::shared_ptr<Figure<int>>>;
template class Array<std::shared_ptr<Figure<double>>>;
template class Array<Square<int>>;
template class Array<Square<double>>;

//Явная инстанциация шаблонных функций
template std::ostream& operator<<<int>(std::ostream&, const Figure<int>&);
template std::istream& operator>><int>(std::istream&, Figure<int>&);
template std::ostream& operator<<<double>(std::ostream&, const Figure<double>&);
template std::istream& operator>><double>(std::istream&, Figure<double>&);
template std::ostream& operator<<<float>(std::ostream&, const Figure<float>&);
template std::istream& operator>><float>(std::istream&, Figure<float>&);

template std::shared_ptr<Figure<int>> create_figure<int>(const std::string&);
template std::shared_ptr<Figure<double>> create_figure<double>(const std::string&);
template std::shared_ptr<Figure<float>> create_figure<float>(const std::string&);

template void print_figure_info<int>(const Figure<int>&);
template void print_figure_info<double>(const Figure<double>&);
template void print_figure_info<float>(const Figure<float>&);

template double calculate_total_area<int>(const Array<std::shared_ptr<Figure<int>>>&);
template double calculate_total_area<double>(const Array<std::shared_ptr<Figure<double>>>&);
template double calculate_total_area<float>(const Array<std::shared_ptr<Figure<float>>>&);

//Явная инстанциация для Array<int>
template class Array<int>;