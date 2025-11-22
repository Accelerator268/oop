#include <gtest/gtest.h>
#include "../include/convert.h"

TEST(test_01, basic_test_set)
{
    ASSERT_TRUE(convertTo24Hour(8, 30, "pm")=="2030");
}

TEST(test_02, basic_test_set)
{
    ASSERT_TRUE(convertTo24Hour(8, 30, "am")=="0830");
}

TEST(test_03, basic_test_set)
{
    ASSERT_TRUE(convertTo24Hour(12, 0, "am")== "0000");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}