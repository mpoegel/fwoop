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
    CounterMetric(const std::string &name, const Labels_t &labels);
    CounterMetric(CounterMetric &rhs) = delete;
    CounterMetric(CounterMetric &&rhs) = default;
    CounterMetric &operator=(CounterMetric &rhs) = default;
    ~CounterMetric() override;

    void increment();

    int count() const;
    uint8_t *encode(Format fmt, unsigned int &length) const override;
};

inline void CounterMetric::increment() { d_count++; }
inline int CounterMetric::count() const { return d_count; }

class CounterMetricSeries : public MetricSeries {
  public:
    CounterMetricSeries(const std::string &name, const std::string &unit, const std::string &summary = "");
    CounterMetricSeries(CounterMetricSeries &rhs) = delete;
    CounterMetricSeries(CounterMetricSeries &&rhs) = default;
    CounterMetricSeries &operator=(CounterMetricSeries &rhs) = default;
    ~CounterMetricSeries() override;

    const char *getType() const override;

    std::shared_ptr<CounterMetric> newCounter(const Metric::Labels_t &labels);
};

inline const char *CounterMetricSeries::getType() const
{
    static constexpr char myType[] = "counter";
    return myType;
}

inline std::shared_ptr<CounterMetric> CounterMetricSeries::newCounter(const Metric::Labels_t &labels)
{
    auto counter = std::make_shared<CounterMetric>(name(), labels);
    addMetric(counter);
    return counter;
}

} // namespace fwoop
