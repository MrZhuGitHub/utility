#include <iostream>
#include <gtest/gtest.h>

int add(const int& a, const int& b)
{
    return a + b;
}

int main() 
{
    testing::InitGoogleTest();
    EXPECT_EQ(add(100, 1000), 1100);
    return RUN_ALL_TESTS();
}