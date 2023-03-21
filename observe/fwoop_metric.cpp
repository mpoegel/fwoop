#include <fwoop_metric.h>

#include <fwoop_log.h>

#include <cstring>

namespace fwoop {

Metric::Metric(const std::string& name, const std::string& unit, const std::string& summary)
: d_name(name)
, d_unit(unit)
, d_summary(summary)
{}

Metric::~Metric()
{}

uint8_t *Metric::encodeMetadata(Format fmt, unsigned int& length) const
{
    static const char HELP[] = "# HELP ";
    static const char UNIT[] = "# UNIT ";
    length = 0;
    if (d_summary.length() > 0) {
        length = strlen(HELP) + 1 + d_name.length() + d_summary.length() + 1;
    };
    length += strlen(UNIT) + 1 + d_name.length() + d_unit.length() + 1;
    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    unsigned int offset = 0;
    if (d_summary.length() > 0) {
        memcpy(encoding + offset, HELP, strlen(HELP));
        offset += strlen(HELP);
        memcpy(encoding + offset, d_name.c_str(), d_name.length());
        offset += d_name.length();
        encoding[offset++] = ' ';
        memcpy(encoding + offset, d_summary.c_str(), d_summary.length());
        offset += d_summary.length();
        encoding[offset++] = '\n';
    }
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

uint8_t *Metric::encodeNameWithLabels(Format fmt, unsigned int& length) const
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

void Metric::addLabel(const std::pair<std::string, std::string>& label)
{
    d_labels.insert(label);
}

} // namespace fwoop
