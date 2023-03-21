#include <fwoop_countermetric.h>

#include <fwoop_log.h>

#include <cstring>

namespace fwoop {

CounterMetric::CounterMetric(const std::string& name, const std::string& unit, const std::string& summary)
: Metric(name, unit, summary)
, d_count(0)
{}

CounterMetric::~CounterMetric()
{}

void CounterMetric::increment()
{
    d_count++;
}

uint8_t *CounterMetric::encode(Format fmt, unsigned int& length) const
{
    static const char TYPE[] = "# TYPE ";
    static const char TYPENAME[] = "counter";
    unsigned int metadataLength;
    uint8_t *metadata = encodeMetadata(fmt, metadataLength);
    length = metadataLength + strlen(TYPE) + d_name.length() + 1 + strlen(TYPENAME) + 1;
    unsigned int nameAndLabelsLength;
    uint8_t *nameAndLabels = encodeNameWithLabels(fmt, nameAndLabelsLength);
    length += nameAndLabelsLength;
    std::string value = std::to_string(d_count);
    length += value.length() + 2;

    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    unsigned int offset = 0;
    memcpy(encoding + offset, metadata, metadataLength);
    offset += metadataLength;
    memcpy(encoding + offset, TYPE, strlen(TYPE));
    offset += strlen(TYPE);
    memcpy(encoding + offset, d_name.c_str(), d_name.length());
    offset += d_name.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, TYPENAME, strlen(TYPENAME));
    offset += strlen(TYPENAME);
    encoding[offset++] = '\n';
    memcpy(encoding + offset, nameAndLabels, nameAndLabelsLength);
    offset += nameAndLabelsLength;
    encoding[offset++] = ' ';
    memcpy(encoding + offset, value.c_str(), value.length());
    offset += value.length();

    encoding[offset++] = '\n';
    delete[] metadata;
    delete[] nameAndLabels;
    return encoding;
}

} // namespace fwoop
