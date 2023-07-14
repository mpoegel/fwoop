#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fwoop {

class Metric {
  public:
    enum Format {
        OpenMetric,
    };

    typedef std::unordered_map<std::string, std::string> Labels_t;

  protected:
    std::string d_name;
    Labels_t d_labels;

    uint8_t *encodeNameWithLabels(Metric::Format fmt, unsigned int &length) const;

  public:
    Metric(const std::string &name, const Labels_t &labels);
    Metric(Metric &rhs) = delete;
    Metric(Metric &&rhs) = default;
    Metric &operator=(Metric &rhs) = default;
    virtual ~Metric() = 0;

    virtual uint8_t *encode(Format fmt, unsigned int &length) const = 0;
};

class MetricSeries {
  private:
    std::string d_name;
    std::string d_unit;
    std::string d_summary;
    std::vector<std::shared_ptr<Metric>> d_metrics;

    uint8_t *encodeMetadata(Metric::Format fmt, unsigned int &length) const;

  protected:
    void addMetric(const std::shared_ptr<Metric> &metric);

  public:
    MetricSeries(const std::string &name, const std::string &unit, const std::string &summary = "");
    MetricSeries(MetricSeries &rhs) = delete;
    MetricSeries(MetricSeries &&rhs) = default;
    MetricSeries &operator=(MetricSeries &rhs) = default;
    virtual ~MetricSeries() = 0;

    virtual const char *getType() const = 0;
    const std::string &name() const;
    uint8_t *encode(Metric::Format fmt, unsigned int &length) const;
};

inline void MetricSeries::addMetric(const std::shared_ptr<Metric> &metric) { d_metrics.push_back(metric); }
inline const std::string &MetricSeries::name() const { return d_name; }

} // namespace fwoop
