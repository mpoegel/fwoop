#include <fwoop_httpresponse.h>

#include <cstring>

namespace fwoop {

HttpResponse::HttpResponse()
: d_status()
{
}

uint8_t *HttpResponse::encode(uint32_t& length) const
{
    static const std::string VERSION = "HTTP/1.1";
    length = VERSION.length() + 1 + d_status.length() + 2;
    for (auto header : d_headers) {
        if (std::holds_alternative<HttpHeader>(header.first)) {
            length += httpHeaderToString(std::get<HttpHeader>(header.first)).length() + 2;
        } else if (std::holds_alternative<HttpCustomHeader>(header.first)) {
            length += std::get<HttpCustomHeader>(header).length() + 2;
        }
        length += header.second.length() + 2;
    }
    length += 2;

    uint8_t *encoding = new uint8_t[length];
    uint32_t offset = 0;
    memcpy(encoding, VERSION.data(), VERSION.length());
    offset += VERSION.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, d_status.data(), d_status.length());
    offset += d_status.length();
    encoding[offset++] = '\r';
    encoding[offset++] = '\n';
    for (auto header : d_headers) {
        if (std::holds_alternative<HttpHeader>(header.first)) {
            const std::string name = httpHeaderToString(std::get<HttpHeader>(header.first));
            memcpy(encoding + offset, name.data(), name.length());
            offset += name.length();
        } else if (std::holds_alternative<HttpCustomHeader>(header.first)) {
            const std::string name = std::get<HttpCustomHeader>(header.first);
            memcpy(encoding + offset, name.data(), name.length());
            offset += name.length();
        }
        encoding[offset++] = ':';
        encoding[offset++] = ' ';
        memcpy(encoding + offset, header.second.data(), header.second.length());
        offset += header.second.length();
        encoding[offset++] = '\r';
        encoding[offset++] = '\n';
    }
    encoding[offset++] = '\r';
    encoding[offset++] = '\n';

    return encoding;
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& response)
{
    os << "[ status=" << response.getStatus() << " ]";
    return os;
}

}
