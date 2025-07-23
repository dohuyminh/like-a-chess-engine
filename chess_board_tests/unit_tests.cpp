#include <gtest/gtest.h>

int main(int argc, char const *argv[])
{
    testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    int result = RUN_ALL_TESTS();
    return result;
}
