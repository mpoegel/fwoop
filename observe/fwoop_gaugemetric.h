#pragma once

#include <fwoop_metric.h>

namespace fwoop {

class GaugeMetric : public Metric {
  private:
    int d_value;

  public:
    GaugeMetric(const std::string &name, const std::string &unit, const std::string &summary = "");
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

} // namespace fwoop
