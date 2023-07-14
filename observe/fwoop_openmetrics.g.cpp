#include <gtest/gtest.h>

#include <fwoop_openmetrics.h>

TEST(OpenMetrics, print)
{
    // GIVEN
    auto metrics = fwoop::OpenMetricsPublisher();

    // WHEN
    auto counterSeries = metrics.newCounterSeries("test_counter", "second", "this is a test counter");
    auto counter = counterSeries->newCounter({{"foo", "bar"}, {"pop", "fizz"}});
    counter->increment();

    auto gaugeSeries = metrics.newGaugeSeries("test_gauge", "celsius");
    auto gauge = gaugeSeries->newGauge({{"rack", "abc"}});
    gauge->set(24);

    // THEN
    metrics.print(std::cerr);
}
