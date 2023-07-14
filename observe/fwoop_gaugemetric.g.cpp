#include <gtest/gtest.h>

#include <fwoop_gaugemetric.h>

TEST(GaugeMetric, Set)
{
    // GIVEN
    auto gaugeSeries = fwoop::GaugeMetricSeries("test_gauge", "units");
    auto gauge = gaugeSeries.newGauge({});

    // WHEN
    gauge->set(42);

    // THEN
    EXPECT_EQ(gauge->get(), 42);
}

TEST(GaugeMetric, Encode)
{
    // GIVEN
    auto gaugeSeries = fwoop::GaugeMetricSeries("test_gauge", "units");
    auto gauge1 = gaugeSeries.newGauge({{"color", "blue"}});
    auto gauge2 = gaugeSeries.newGauge({{"color", "red"}});
    unsigned int length;
    const std::string expected = "# HELP test_gauge\n"
                                 "# TYPE test_gauge gauge\n"
                                 "# UNIT test_gauge units\n"
                                 "test_gauge{color=\"blue\"} 42\n"
                                 "test_gauge{color=\"red\"} 500\n";

    // WHEN
    gauge1->set(42);
    gauge2->set(500);

    // THEN
    auto encoding = gaugeSeries.encode(fwoop::Metric::Format::OpenMetric, length);
    const std::string actual((char *)encoding, length);
    std::cerr << actual << '\n';
    EXPECT_EQ(actual, expected);

    delete[] encoding;
}
