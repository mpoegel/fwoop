#pragma once

#include "fwoop_countermetric.h"
#include <fwoop_metric.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fwoop {

class GaugeMetric : public Metric {
  private:
    int d_value;

  public:
    GaugeMetric(const std::string &name, const Labels_t &labels);
    GaugeMetric(GaugeMetric &rhs) = delete;
    GaugeMetric(GaugeMetric &&rhs) = default;
    GaugeMetric &operator=(GaugeMetric &rhs) = default;
    ~GaugeMetric() override;

    void set(int newValue);

    int get() const;
    uint8_t *encode(Format fmt, unsigned int &length) const override;
};

inline void GaugeMetric::set(int newValue) { d_value = newValue; }
inline int GaugeMetric::get() const { return d_value; }

class GaugeMetricSeries : public MetricSeries {
  public:
    GaugeMetricSeries(const std::string &name, const std::string &unit, const std::string &summary = "");
    GaugeMetricSeries(GaugeMetricSeries &rhs) = delete;
    GaugeMetricSeries(GaugeMetricSeries &&rhs) = default;
    GaugeMetricSeries &operator=(GaugeMetricSeries &rhs) = default;
    ~GaugeMetricSeries() override;

    const char *getType() const override;

    std::shared_ptr<GaugeMetric> newGauge(const Metric::Labels_t &labels);
};

inline const char *GaugeMetricSeries::getType() const
{
    static constexpr char myType[] = "gauge";
    return myType;
}

inline std::shared_ptr<GaugeMetric> GaugeMetricSeries::newGauge(const Metric::Labels_t &labels)
{
    auto gauge = std::make_shared<GaugeMetric>(name(), labels);
    addMetric(gauge);
    return gauge;
}

} // namespace fwoop
