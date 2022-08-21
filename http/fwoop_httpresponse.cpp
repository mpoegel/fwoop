#include <fwoop_httpresponse.h>

#include <cstring>

#include <fwoop_filereader.h>
#include <fwoop_log.h>

namespace fwoop {

HttpResponse::HttpResponse()
: d_status()
{
}

uint8_t *HttpResponse::encode(uint32_t& length) const
{
    static const std::string VERSION = "HTTP/1.1";
    length = VERSION.length() + 1 + d_status.length() + 2;

    uint8_t* fileContents = nullptr;
    uint32_t contentLength;
    if (d_fileName.length() > 0) {
        FileReader reader(d_fileName);
        if (0 != reader.open()) {
            Log::Error("could not open file=", d_fileName);
        } else {
            fileContents = reader.loadFile(contentLength);
        }
    } else {
        contentLength = d_body.length();
    }
    length += contentLength;

    std::vector<HttpHeaderField_t> headers(d_headers);
    headers.push_back({HttpHeader::ContentLength, std::to_string(contentLength)});

    for (auto header : headers) {
        if (std::holds_alternative<HttpHeader>(header.first)) {
            length += httpHeaderToString(std::get<HttpHeader>(header.first)).length() + 2;
        } else if (std::holds_alternative<HttpCustomHeader>(header.first)) {
            length += std::get<HttpCustomHeader>(header).length() + 2;
        }
        length += header.second.length() + 2;
    }
    length += 2;

    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    uint32_t offset = 0;
    memcpy(encoding, VERSION.data(), VERSION.length());
    offset += VERSION.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, d_status.data(), d_status.length());
    offset += d_status.length();
    encoding[offset++] = '\r';
    encoding[offset++] = '\n';
    for (auto header : headers) {
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

    if (nullptr != fileContents) {
        memcpy(encoding + offset, fileContents, contentLength);
    } else {
        memcpy(encoding + offset, d_body.data(), d_body.length());
    }
    offset += contentLength;
    return encoding;
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& response)
{
    os << "[ status=" << response.getStatus() << " headers=[ ";
    auto headers = response.getHeaders();
    for (unsigned int i = 0; i < headers.size(); ++i) {
        os << "[ name=" << headers[i].first << " value=" << headers[i].second << " ]";
        if (i < headers.size() - 1) {
            os << ", ";
        }
    }
    os << " ] file=" << response.getFile() << " body=" << response.getBody() << " ]";
    return os;
}

}
