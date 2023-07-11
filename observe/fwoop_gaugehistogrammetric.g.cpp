#include <fwoop_gaugehistogrammetric.h>

#include <cstdint>

#include <gtest/gtest.h>

TEST(GaugeHistogramMetric, record)
{
    // GIVEN
    auto metric = fwoop::GaugeHistogramMetric("histogram_test", "seconds", 4, 10, 3);

    // WHEN
    metric.record(4, 1);
    metric.record(6, 2);
    metric.record(8, 1);

    // THEN
    EXPECT_EQ(1, metric.getBin(0));
    EXPECT_EQ(2, metric.getBin(1));
    EXPECT_EQ(1, metric.getBin(2));
}

TEST(GaugeHistogramMetric, encode)
{
    // GIVEN
    auto metric = fwoop::GaugeHistogramMetric("histogram_test", "seconds", 4, 10, 3);
    metric.addLabel(std::pair<std::string, std::string>{"color", "blue"});
    unsigned int length = 0;

    // WHEN
    metric.record(4, 1);
    metric.record(6, 2);
    metric.record(8, 1);

    // THEN
    auto encoding = metric.encode(fwoop::Metric::Format::OpenMetric, length);
    std::cerr << std::string((char *)encoding, length) << '\n';
    delete[] encoding;
}
