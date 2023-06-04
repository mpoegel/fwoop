#include <fwoop_datetime.h>

#include <gtest/gtest.h>

TEST(DateTime, 2023_June_4_15_0_0)
{
    // GIVEN
    uint32_t time = 1685890800;

    // WHEN
    auto dt = fwoop::DateTime(time);

    // THEN
    EXPECT_EQ(2023, dt.year());
    EXPECT_EQ(5, dt.month());
    EXPECT_EQ(4, dt.day());
    EXPECT_EQ(15, dt.hour());
    EXPECT_EQ(0, dt.minute());
    EXPECT_EQ(0, dt.second());
}

TEST(DateTime, 2020_Mar_1_11_59_59)
{
    // GIVEN
    uint32_t time = 1583063999;

    // WHEN
    auto dt = fwoop::DateTime(time);

    // THEN
    EXPECT_EQ(2020, dt.year());
    EXPECT_EQ(2, dt.month());
    EXPECT_EQ(1, dt.day());
    EXPECT_EQ(11, dt.hour());
    EXPECT_EQ(59, dt.minute());
    EXPECT_EQ(59, dt.second());
}
