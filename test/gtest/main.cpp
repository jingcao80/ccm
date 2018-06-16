//testgcd.cpp
#include <gtest/gtest.h>
int Gcd(int a, int b)    //计算最大公约数
{
    return 0 == b ? a : Gcd(b, a % b);
}
TEST(GcdTest, IntTest)
{
    EXPECT_EQ(1, Gcd(2, 5));
    EXPECT_EQ(2, Gcd(2, 5));
    EXPECT_EQ(2, Gcd(2, 4));
    EXPECT_EQ(3, Gcd(6, 9));
}
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}