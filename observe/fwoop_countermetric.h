#pragma once

#include <fwoop_metric.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace fwoop {

class CounterMetric : public Metric {
  private:
    int d_count;

  public:
    CounterMetric(const std::string &name, const std::string &unit, const std::string &summary = "");
    CounterMetric(CounterMetric &rhs) = delete;
    CounterMetric(CounterMetric &&rhs) = default;
    CounterMetric &operator=(CounterMetric &rhs) = default;
    ~CounterMetric() override;

    void increment();

    int count() const;
    uint8_t *encode(Format fmt, unsigned int &length) const override;
};

inline int CounterMetric::count() const { return d_count; }

} // namespace fwoop
