#include <gtest/gtest.h>
#include "../include/money.h"

TEST(MoneyTest, PositiveAmountCreation)
{
    Money m1("123.45");
    std::cout << m1.toString();
    ASSERT_TRUE(m1.toString() == "123.45");
}

TEST(MoneyTest, NegativeAmountCreation)
{
    Money m3("-50.00");
    ASSERT_TRUE(m3.toString() == "-50.00");
    std::cout << m3.toString();
    ASSERT_TRUE(m3.isNegative());
}

TEST(MoneyTest, AdditionOperation)
{
    Money m1("123.45");
    Money m2("67.89");
    Money sum = m1.add(m2);
    std::cout << sum.toString();
    ASSERT_TRUE(sum.toString() == "191.34");
}

TEST(MoneyTest, SubtractionOperation)
{
    Money m1("100.00");
    Money m2("50.50");
    Money diff = m1.subtract(m2);
    ASSERT_TRUE(diff.toString() == "49.50");
}

TEST(MoneyTest, MultiplicationOperation)
{
    Money m1("25.50");
    Money product = m1.multiply(3);
    ASSERT_TRUE(product.toString() == "76.50");
}

TEST(MoneyTest, DivisionOperation)
{
    Money m1("100.00");
    Money quotient = m1.divide(4);
    ASSERT_TRUE(quotient.toString() == "25.00");
}

TEST(MoneyTest, ComparisonOperations)
{
    Money m1("100.00");
    Money m2("50.00");
    Money m3("100.00");
    
    ASSERT_TRUE(m1.greaterThan(m2));
    ASSERT_TRUE(m2.lessThan(m1));
    ASSERT_TRUE(m1.equals(m3));
}

TEST(MoneyTest, ZeroAmount)
{
    Money zero;
    ASSERT_TRUE(zero.toString() == "0.00");
    ASSERT_FALSE(zero.isNegative());
}

TEST(MoneyTest, NegativeAddition)
{
    Money m1("100.00");
    Money m3("-25.00");
    Money result = m1.add(m3);
    ASSERT_TRUE(result.toString() == "75.00");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}