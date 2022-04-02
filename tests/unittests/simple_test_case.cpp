#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(unit_dummy, simple_test_case)
{
    // Empty test for now to ensure it can run.
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

