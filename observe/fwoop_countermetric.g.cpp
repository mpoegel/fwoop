#include <gtest/gtest.h>

#include <fwoop_countermetric.h>

TEST(CounterMetric, Count)
{
    // GIVEN
    auto counterSeries = fwoop::CounterMetricSeries("test_metric", "second");
    auto counter = counterSeries.newCounter({});

    // WHEN
    counter->increment();

    // THEN
    EXPECT_EQ(1, counter->count());
}

TEST(CounterMetric, Encode)
{
    // GIVEN
    auto counterSeries = fwoop::CounterMetricSeries("test_counter", "units");
    auto counter1 = counterSeries.newCounter({{"color", "blue"}});
    auto counter2 = counterSeries.newCounter({{"color", "red"}});
    unsigned int length;
    const std::string expected = "# HELP test_counter\n"
                                 "# TYPE test_counter counter\n"
                                 "# UNIT test_counter units\n"
                                 "test_counter{color=\"blue\"} 1\n"
                                 "test_counter{color=\"red\"} 3\n";

    // WHEN
    counter1->increment();
    counter2->increment();
    counter2->increment();
    counter2->increment();

    // THEN
    auto encoding = counterSeries.encode(fwoop::Metric::Format::OpenMetric, length);
    const std::string actual((char *)encoding, length);
    std::cerr << actual << '\n';
    EXPECT_EQ(actual, expected);

    delete[] encoding;
}
