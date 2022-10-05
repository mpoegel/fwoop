#pragma once

#include <string>

namespace fwoop {

class HttpVersion {
    public:
    enum Value {
        Undefined,
        Http1_0,
        Http1_1,
        Http2,
    };

    static HttpVersion::Value fromString(const std::string& versionStr);
    static std::string toString(const HttpVersion::Value version);

    private:
    HttpVersion() = delete;
};

std::ostream& operator<<(std::ostream& os, HttpVersion::Value version);

}
