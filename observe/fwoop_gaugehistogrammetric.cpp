#include <fwoop_gaugehistogrammetric.h>

#include <cstdint>
#include <cstring>
#include <string>

namespace fwoop {

GaugeHistogramMetric::~GaugeHistogramMetric() {}

void GaugeHistogramMetric::reset() { d_buckets.clear(); }

const int32_t &GaugeHistogramMetric::getBin(uint16_t bucketNum) { return d_buckets[bucketNum]; }

GaugeHistogramMetric::GaugeHistogramMetric(const std::string &name, int32_t low, int32_t high, uint16_t numBuckets,
                                           const Labels_t &labels)
    : Metric(name, labels), d_low(low), d_high(high), d_numBuckets(numBuckets),
      d_stepSize(double(d_high - d_low) / d_numBuckets), d_buckets(d_numBuckets, int32_t())
{
}

bool GaugeHistogramMetric::record(int32_t determinant, int32_t value)
{
    if (determinant < d_low || determinant > d_high) {
        return false;
    }
    uint16_t bin = std::floor((determinant - d_low) / d_stepSize);
    d_buckets[bin] = value;
    return true;
}

void GaugeHistogramMetric::encodeBucket(const uint16_t bucketNum, uint8_t *out, const uint32_t outLen,
                                        uint32_t &encodedLen) const
{
    static const char BUCKET[] = "_bucket";
    static const char OPEN_LABEL[] = "{le=";
    static const char CLOSE_LABEL[] = "} ";

    encodedLen = 0;
    std::string le;
    if (bucketNum == d_buckets.size() - 1) {
        le = "+Inf";
    } else {
        le = std::to_string(d_low + (d_stepSize * bucketNum));
    }
    auto value = std::to_string(d_buckets[bucketNum]);
    if (d_name.length() + strlen(BUCKET) + strlen(OPEN_LABEL) + le.size() + value.size() + 1 >= outLen) {
        // not enough space
        return;
    }
    memcpy(out, d_name.c_str(), d_name.length());
    encodedLen += d_name.length();
    memcpy(out + encodedLen, BUCKET, strlen(BUCKET));
    encodedLen += strlen(BUCKET);
    memcpy(out + encodedLen, OPEN_LABEL, strlen(OPEN_LABEL));
    encodedLen += strlen(OPEN_LABEL);
    out[encodedLen++] = '"';
    memcpy(out + encodedLen, le.c_str(), le.size());
    encodedLen += le.size();
    out[encodedLen++] = '"';

    for (auto lbl : d_labels) {
        out[encodedLen++] = ',';
        memcpy(out + encodedLen, lbl.first.c_str(), lbl.first.length());
        encodedLen += lbl.first.length();
        out[encodedLen++] = '=';
        out[encodedLen++] = '"';
        memcpy(out + encodedLen, lbl.second.c_str(), lbl.second.length());
        encodedLen += lbl.second.length();
        out[encodedLen++] = '"';
    }

    memcpy(out + encodedLen, CLOSE_LABEL, strlen(CLOSE_LABEL));
    encodedLen += strlen(CLOSE_LABEL);
    memcpy(out + encodedLen, value.c_str(), value.size());
    encodedLen += value.size();
    out[encodedLen++] = '\n';
}

uint8_t *GaugeHistogramMetric::encode(Format fmt, unsigned int &length) const
{
    length = 0;
    uint32_t bufferSize = 1024;
    uint8_t *buffer = new uint8_t[bufferSize];
    uint32_t offset = 0;
    for (unsigned int b = 0; b < d_numBuckets; b++) {
        uint32_t encodedLen = 0;
        encodeBucket(b, buffer + offset, bufferSize - offset, encodedLen);
        if (encodedLen == 0) {
            // not enough room, double buffer and try again
            bufferSize *= 2;
            uint8_t *newBuffer = new uint8_t[bufferSize];
            memcpy(newBuffer, buffer, offset);
            delete[] buffer;
            buffer = newBuffer;
            b--;
        } else {
            offset += encodedLen;
        }
    }
    length += offset;

    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    memcpy(encoding, buffer, offset);

    return encoding;
}

GaugeHistogramMetricSeries::GaugeHistogramMetricSeries(const std::string &name, const std::string &unit,
                                                       const std::string &summary)
    : MetricSeries(name, unit, summary)
{
}

GaugeHistogramMetricSeries::~GaugeHistogramMetricSeries() {}

} // namespace fwoop
