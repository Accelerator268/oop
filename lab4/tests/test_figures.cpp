#include "../include/figure.h"
#include <gtest/gtest.h>
#include <sstream>

//Тесты для Point
TEST(PointTest, CreationAndAccess) {
    Point<int> p1(1, 2);
    EXPECT_EQ(p1.x(), 1);
    EXPECT_EQ(p1.y(), 2);
    
    Point<double> p2(3.5, 4.5);
    EXPECT_DOUBLE_EQ(p2.x(), 3.5);
    EXPECT_DOUBLE_EQ(p2.y(), 4.5);
}

TEST(PointTest, Comparison) {
    Point<int> p1(1, 2);
    Point<int> p2(1, 2);
    Point<int> p3(3, 4);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
    EXPECT_TRUE(p1 != p3);
}

TEST(PointTest, IOStream) {
    Point<int> p(10, 20);
    std::stringstream ss;
    ss << p;
    EXPECT_EQ(ss.str(), "(10, 20)");
    
    std::stringstream ss2("30 40");
    Point<int> p2;
    ss2 >> p2;
    EXPECT_EQ(p2.x(), 30);
    EXPECT_EQ(p2.y(), 40);
}

//Тесты для Square
TEST(SquareTest, Creation) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {1, 0}, {1, 1}, {0, 1}
    };
    
    Square<double> square(vertices);
    EXPECT_DOUBLE_EQ(square.area(), 1.0);
}

TEST(SquareTest, Area) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {2, 0}, {2, 2}, {0, 2}
    };
    
    Square<double> square(vertices);
    EXPECT_DOUBLE_EQ(square.area(), 4.0);
}

TEST(SquareTest, GeometricCenter) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {2, 0}, {2, 2}, {0, 2}
    };
    
    Square<double> square(vertices);
    auto center = square.geometric_center();
    EXPECT_DOUBLE_EQ(center.x(), 1.0);
    EXPECT_DOUBLE_EQ(center.y(), 1.0);
}

TEST(SquareTest, Clone) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {1, 0}, {1, 1}, {0, 1}
    };
    
    Square<double> square(vertices);
    auto cloned = square.clone();
    
    EXPECT_TRUE(*cloned == square);
}

//Тесты для Rectangle
TEST(RectangleTest, Creation) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {2, 0}, {2, 1}, {0, 1}
    };
    
    Rectangle<double> rectangle(vertices);
    EXPECT_NEAR(rectangle.area(), 2.0, 1e-10);
}

TEST(RectangleTest, Area) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {3, 0}, {3, 2}, {0, 2}
    };
    
    Rectangle<double> rectangle(vertices);
    EXPECT_NEAR(rectangle.area(), 6.0, 1e-10);
}

//Тесты для Trapezoid
TEST(TrapezoidTest, Creation) {
    std::vector<Point<double>> vertices = {
        {0, 0}, {3, 0}, {2, 2}, {1, 2}
    };
    
    Trapezoid<double> trapezoid(vertices);
    EXPECT_GT(trapezoid.area(), 0.0);
}

//Тесты для Array
TEST(ArrayTest, BasicOperations) {
    Array<int> arr;
    EXPECT_TRUE(arr.empty());
    
    arr.push_back(10);
    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0], 10);
    
    arr.push_back(20);
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[1], 20);
}

TEST(ArrayTest, Erase) {
    Array<int> arr;
    arr.push_back(10);
    arr.push_back(20);
    arr.push_back(30);
    
    arr.erase(1);
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1], 30);
}

TEST(ArrayTest, MoveOperations) {
    Array<int> arr1;
    arr1.push_back(1);
    arr1.push_back(2);
    arr1.push_back(3);
    
    Array<int> arr2 = std::move(arr1);
    EXPECT_EQ(arr1.size(), 0);
    EXPECT_EQ(arr2.size(), 3);
    EXPECT_EQ(arr2[0], 1);
}

//Тесты для фабрики
TEST(FactoryTest, CreateFigures) {
    auto square = create_figure<double>("square");
    EXPECT_NE(dynamic_cast<Square<double>*>(square.get()), nullptr);
    
    auto rectangle = create_figure<double>("rectangle");
    EXPECT_NE(dynamic_cast<Rectangle<double>*>(rectangle.get()), nullptr);
    
    auto trapezoid = create_figure<double>("trapezoid");
    EXPECT_NE(dynamic_cast<Trapezoid<double>*>(trapezoid.get()), nullptr);
}

TEST(FactoryTest, InvalidType) {
    EXPECT_THROW(create_figure<double>("invalid"), std::invalid_argument);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}