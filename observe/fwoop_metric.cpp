#include <cstdint>
#include <fwoop_metric.h>

#include <fwoop_log.h>

#include <cstring>

namespace fwoop {

Metric::Metric(const std::string &name, const std::unordered_map<std::string, std::string> &labels)
    : d_name(name), d_labels(labels)
{
}

Metric::~Metric() {}

uint8_t *Metric::encodeNameWithLabels(Format fmt, unsigned int &length) const
{
    length = d_name.length() + 2;
    for (auto label : d_labels) {
        length += label.first.length() + label.second.length() + 3;
    }
    length += d_labels.size() > 0 ? d_labels.size() - 1 : 0;

    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    unsigned int offset = 0;
    memcpy(encoding + offset, d_name.c_str(), d_name.length());
    offset += d_name.length();
    encoding[offset++] = '{';
    bool first = true;
    for (auto label : d_labels) {
        if (!first) {
            encoding[offset++] = ',';
        }
        memcpy(encoding + offset, label.first.c_str(), label.first.length());
        offset += label.first.length();
        encoding[offset++] = '=';
        encoding[offset++] = '"';
        memcpy(encoding + offset, label.second.c_str(), label.second.length());
        offset += label.second.length();
        encoding[offset++] = '"';
        first = false;
    }
    encoding[offset++] = '}';
    return encoding;
}

MetricSeries::MetricSeries(const std::string &name, const std::string &unit, const std::string &summary)
    : d_name(name), d_unit(unit), d_summary(summary)
{
}

MetricSeries::~MetricSeries() {}

uint8_t *MetricSeries::encodeMetadata(Metric::Format fmt, unsigned int &length) const
{
    static const char HELP[] = "# HELP ";
    static const char UNIT[] = "# UNIT ";
    static const char TYPE[] = "# TYPE ";
    length = strlen(HELP) + 1 + d_name.length();
    if (d_summary.length() > 0) {
        length += d_summary.length() + 1;
    };
    length += strlen(UNIT) + 1 + d_name.length() + d_unit.length() + 1;
    length += strlen(TYPE) + 1 + d_name.length() + strlen(getType()) + 1;
    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    unsigned int offset = 0;
    memcpy(encoding + offset, HELP, strlen(HELP));
    offset += strlen(HELP);
    memcpy(encoding + offset, d_name.c_str(), d_name.length());
    offset += d_name.length();
    if (d_summary.length() > 0) {
        encoding[offset++] = ' ';
        memcpy(encoding + offset, d_summary.c_str(), d_summary.length());
        offset += d_summary.length();
    }
    encoding[offset++] = '\n';

    memcpy(encoding + offset, TYPE, strlen(TYPE));
    offset += strlen(TYPE);
    memcpy(encoding + offset, d_name.c_str(), d_name.length());
    offset += d_name.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, getType(), strlen(getType()));
    offset += strlen(getType());
    encoding[offset++] = '\n';

    memcpy(encoding + offset, UNIT, strlen(UNIT));
    offset += strlen(UNIT);
    memcpy(encoding + offset, d_name.c_str(), d_name.length());
    offset += d_name.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, d_unit.c_str(), d_unit.length());
    offset += d_unit.length();
    encoding[offset++] = '\n';
    return encoding;
}

uint8_t *MetricSeries::encode(Metric::Format fmt, unsigned int &length) const
{
    unsigned int offset;
    uint8_t *metadata = encodeMetadata(fmt, offset);
    unsigned int bufSize = 1024;
    uint8_t *buf = new uint8_t[bufSize];
    // TODO handle case where metadata is bigger than starting buf size
    memcpy(buf, metadata, offset);
    delete[] metadata;
    unsigned int metricLen;
    for (auto metric : d_metrics) {
        uint8_t *m = metric->encode(fmt, metricLen);
        if (offset + metricLen > bufSize) {
            // resize
            unsigned int newBufSize = bufSize * 2;
            uint8_t *newBuf = new uint8_t[newBufSize];
            memcpy(newBuf, buf, offset);
            // TODO ensure resize makes enough space
            delete[] buf;
            buf = newBuf;
            bufSize = newBufSize;
        }
        memcpy(buf + offset, m, metricLen);
        offset += metricLen;
        delete[] m;
    }
    length = offset;
    return buf;
}

} // namespace fwoop
