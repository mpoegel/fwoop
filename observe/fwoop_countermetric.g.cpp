#include <gtest/gtest.h>

#include <fwoop_countermetric.h>

TEST(CounterMetric, Count)
{
    // GIVEN
    auto counter = fwoop::CounterMetric("test_metric", "second");

    // WHEN
    counter.increment();

    // THEN
    EXPECT_EQ(1, counter.count());
}
