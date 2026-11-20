#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Basic test to ensure test framework works
class BasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for each test
    }

    void TearDown() override {
        // Cleanup for each test
    }
};

TEST_F(BasicTest, TestFrameworkWorks) {
    EXPECT_TRUE(true);
    EXPECT_EQ(2 + 2, 4);
    EXPECT_NE(2 + 2, 5);
}

TEST_F(BasicTest, StringComparison) {
    std::string hello = "Hello";
    std::string world = "World";

    EXPECT_NE(hello, world);
    EXPECT_EQ(hello.length(), 5);
    EXPECT_EQ(world.length(), 5);
}

TEST_F(BasicTest, MathematicalOperations) {
    int a = 10;
    int b = 20;

    EXPECT_EQ(a + b, 30);
    EXPECT_EQ(b - a, 10);
    EXPECT_EQ(a * 2, b);
    EXPECT_EQ(b / 2, a);
}

TEST_F(BasicTest, BooleanLogic) {
    bool trueValue = true;
    bool falseValue = false;

    EXPECT_TRUE(trueValue);
    EXPECT_FALSE(falseValue);
    EXPECT_TRUE(trueValue && !falseValue);
    EXPECT_TRUE(trueValue || falseValue);
    EXPECT_FALSE(trueValue && falseValue);
}

// Test with different types
class TypeTest : public ::testing::Test {};

TEST_F(TypeTest, IntegerTypes) {
    int8_t small = 127;
    int16_t medium = 32767;
    int32_t large = 2147483647;
    int64_t huge = 9223372036854775807LL;

    EXPECT_EQ(sizeof(small), 1);
    EXPECT_EQ(sizeof(medium), 2);
    EXPECT_EQ(sizeof(large), 4);
    EXPECT_EQ(sizeof(huge), 8);
}

TEST_F(TypeTest, FloatingPointTypes) {
    float f = 3.14f;
    double d = 3.141592653589793;

    EXPECT_FLOAT_EQ(f, 3.14f);
    EXPECT_DOUBLE_EQ(d, 3.141592653589793);
    EXPECT_NE(static_cast<double>(f), d); // float has less precision
}