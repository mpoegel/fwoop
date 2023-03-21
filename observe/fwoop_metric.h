#pragma once

#include <string>
#include <unordered_map>

namespace fwoop {

class Metric {
    public:
    enum Format {
        OpenMetric,
    };
    
    protected:
    std::string d_name;
    std::string d_unit;
    std::string d_summary;
    std::unordered_map<std::string, std::string> d_labels;

    uint8_t *encodeMetadata(Format fmt, unsigned int& length) const;
    uint8_t *encodeNameWithLabels(Format fmt, unsigned int& length) const;

    public:
    Metric(const std::string& name, const std::string& unit, const std::string& summary="");
    Metric(Metric& rhs) = delete;
    Metric(Metric&& rhs) = default;
    Metric& operator=(Metric& rhs) = default;
    virtual ~Metric() = 0;

    void addLabel(const std::pair<std::string, std::string>& label);

    virtual uint8_t *encode(Format fmt, unsigned int& length) const = 0;
};

} // namespace fwoop
