// tests.cpp
#include "blang_test.cpp"
#include <gtest/gtest.h>

TEST(BlangTest, PositiveNos) 
{
    std::string infile = "cmdfile.txt";
    ASSERT_EQ(0, blangParse(infile));
}

TEST(BlangTest, NegativeNos) 
{
    std::string infile = "not_here.txt";
    ASSERT_EQ(-1.0, blangParse(infile));
}

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
