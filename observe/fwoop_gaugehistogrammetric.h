#pragma once

#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

#include <fwoop_metric.h>

namespace fwoop {

class GaugeHistogramMetric : public Metric {
  private:
    int32_t d_low;
    int32_t d_high;
    uint16_t d_numBuckets;
    double d_stepSize;
    std::vector<int32_t> d_buckets;

    void encodeBucket(const uint16_t bucketNum, uint8_t *out, const uint32_t outLen, uint32_t &encodedLen) const;

  public:
    GaugeHistogramMetric(const std::string &name, const std::string &unit, int32_t low, int32_t high,
                         uint16_t numBuckets, const std::string &summary = "");
    GaugeHistogramMetric(GaugeHistogramMetric &rhs) = delete;
    GaugeHistogramMetric(GaugeHistogramMetric &&rhs) = default;
    GaugeHistogramMetric &operator=(GaugeHistogramMetric &rhs) = default;
    ~GaugeHistogramMetric() override;

    bool record(int32_t determinant, int32_t value);
    void reset();
    const int32_t &getBin(uint16_t bucketNum);

    uint8_t *encode(Format fmt, unsigned int &length) const override;
};

} // namespace fwoop
