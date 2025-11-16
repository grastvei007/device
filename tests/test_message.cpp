#include <gtest/gtest.h>

class TestMessge : public ::testing::Test
{
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TestMessge, tesdf) {}
