#include <fwoop_gaugehistogrammetric.h>

#include <cstdint>

#include <gtest/gtest.h>

TEST(GaugeHistogramMetric, record)
{
    // GIVEN
    auto series = fwoop::GaugeHistogramMetricSeries("histogram_test", "seconds");
    auto metric = series.newGaugeHistogram(4, 10, 3, {});

    // WHEN
    metric->record(4, 1);
    metric->record(6, 2);
    metric->record(8, 1);

    // THEN
    EXPECT_EQ(1, metric->getBin(0));
    EXPECT_EQ(2, metric->getBin(1));
    EXPECT_EQ(1, metric->getBin(2));
}

TEST(GaugeHistogramMetric, encode)
{
    // GIVEN
    auto series = fwoop::GaugeHistogramMetricSeries("histogram_test", "seconds");
    auto metric = series.newGaugeHistogram(4, 10, 3, {{"color", "blue"}});
    unsigned int length = 0;
    const std::string expected = "# HELP histogram_test\n"
                                 "# TYPE histogram_test gaugehistogram\n"
                                 "# UNIT histogram_test seconds\n"
                                 "histogram_test_bucket{le=\"4.000000\",color=\"blue\"} 1\n"
                                 "histogram_test_bucket{le=\"6.000000\",color=\"blue\"} 2\n"
                                 "histogram_test_bucket{le=\"+Inf\",color=\"blue\"} 1\n";

    // WHEN
    metric->record(4, 1);
    metric->record(6, 2);
    metric->record(8, 1);

    // THEN
    auto encoding = series.encode(fwoop::Metric::Format::OpenMetric, length);
    const std::string actual((char *)encoding, length);
    std::cerr << actual << '\n';
    EXPECT_EQ(actual, expected);

    delete[] encoding;
}
