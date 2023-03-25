#include <gtest/gtest.h>

#include <fwoop_openmetrics.h>

TEST(OpenMetrics, print)
{
    // GIVEN
    auto metrics = fwoop::OpenMetricsPublisher();

    // WHEN
    auto counter = metrics.newCounter("test_counter", "second", "this is a test counter");
    counter->addLabel({"foo", "bar"});
    counter->addLabel({"pop", "fizz"});
    counter->increment();

    auto gauge = metrics.newGauge("test_gauge", "celsius");
    gauge->addLabel({"rack", "abc"});
    gauge->set(24);

    // THEN
    metrics.print(std::cerr);
}
