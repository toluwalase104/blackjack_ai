#include <gtest/gtest.h>

// Basic starter test
TEST(HelloTest, BasicAssertions) {
    // Expect string inequality
    EXPECT_STRNE("hello", "world");

    // Expect equality
    EXPECT_EQ(7 * 6, 42);
}