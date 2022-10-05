#include <fwoop_httpversion.h>

namespace fwoop {

HttpVersion::Value HttpVersion::fromString(const std::string& versionStr)
{
    if (versionStr == "HTTP/1.0") return Value::Http1_0;
    if (versionStr == "HTTP/1.1") return Value::Http1_1;
    if (versionStr == "HTTP/2") return Value::Http2;
    return Value::Undefined;
}

std::string HttpVersion::toString(const HttpVersion::Value version)
{
    switch (version) {
        case HttpVersion::Value::Http1_0: return "HTTP/1.0";
        case HttpVersion::Value::Http1_1: return "HTTP/1.1";
        case HttpVersion::Value::Http2: return "HTTP/2";
        default: return "Undefined";
    }
}

std::ostream& operator<<(std::ostream& os, HttpVersion::Value version)
{
    os << HttpVersion::toString(version);
    return os;
}

}
