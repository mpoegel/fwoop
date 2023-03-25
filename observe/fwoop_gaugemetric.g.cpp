#include <gtest/gtest.h>

#include <fwoop_gaugemetric.h>

TEST(GaugeMetric, Set)
{
    // GIVEN
    auto gauge = fwoop::GaugeMetric("test_gauge", "units");

    // WHEN
    gauge.set(42);

    // THEN
    EXPECT_EQ(gauge.get(), 42);
}
