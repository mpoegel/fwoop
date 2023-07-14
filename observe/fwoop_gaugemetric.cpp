#include "fwoop_metric.h"
#include <fwoop_gaugemetric.h>

#include <cstring>

namespace fwoop {

GaugeMetric::GaugeMetric(const std::string &name, const Labels_t &labels) : Metric(name, labels), d_value(0) {}

GaugeMetric::~GaugeMetric() {}

uint8_t *GaugeMetric::encode(Format fmt, unsigned int &length) const
{
    unsigned int nameAndLabelsLength;
    uint8_t *nameAndLabels = encodeNameWithLabels(fmt, nameAndLabelsLength);
    length = nameAndLabelsLength;
    std::string value = std::to_string(d_value);
    length += value.length() + 2;

    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    unsigned int offset = 0;
    memcpy(encoding + offset, nameAndLabels, nameAndLabelsLength);
    offset += nameAndLabelsLength;
    encoding[offset++] = ' ';
    memcpy(encoding + offset, value.c_str(), value.length());
    offset += value.length();

    encoding[offset++] = '\n';
    delete[] nameAndLabels;
    return encoding;
}

GaugeMetricSeries::GaugeMetricSeries(const std::string &name, const std::string &unit, const std::string &summary)
    : MetricSeries(name, unit, summary)
{
}

GaugeMetricSeries::~GaugeMetricSeries() {}

} // namespace fwoop
