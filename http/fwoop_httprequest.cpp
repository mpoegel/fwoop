#include <fwoop_httprequest.h>

#include <cstring>

#include <fwoop_log.h>
#include <fwoop_tokenizer.h>

namespace fwoop {

std::ostream& operator<<(std::ostream& os, HttpRequest::Method method)
{
    os << HttpRequest::methodToString(method);
    return os;
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& request)
{
    os << "[ method=" << request.getMethod() << " path=" << request.getPath() << " version=" << request.getVersion()
       << " canUpgrade=" << request.canUpgrade() << " headers=[ ";
    auto headers = request.getHeaders();
    for (unsigned int i = 0; i < headers.size(); ++i) {
        os << "[ name=" << headers[i].first << " value=" << headers[i].second << " ]";
        if (i < headers.size() - 1) {
            os << ", ";
        }
    }
    os << " ] ]";
    return os;
}

HttpRequest::HttpRequest()
: d_method(Method::Undefined)
, d_path()
, d_version(HttpVersion::Value::Undefined)
, d_canUpgrade(false)
{
}

HttpRequest::Method HttpRequest::stringToMethod(const std::string& methodStr)
{
    if (methodStr == "GET") return Method::Get;
    if (methodStr == "POST") return Method::Post;
    if (methodStr == "PUT") return Method::Put;
    if (methodStr == "DELETE") return Method::Delete;
    return Method::Undefined;
}

std::string HttpRequest::methodToString(const Method& method)
{
    switch (method) {
        case HttpRequest::Method::Get: return "GET";
        case HttpRequest::Method::Post: return "POST";
        case HttpRequest::Method::Put: return "PUT";
        case HttpRequest::Method::Delete: return "DELETE";
        default: return "Undefined";
    }
}

std::shared_ptr<HttpRequest> HttpRequest::parse(uint8_t *buffer, uint32_t bufferSize, uint32_t& bytesParsed)
{
    auto request = std::make_shared<HttpRequest>();
    bytesParsed = 0;

    std::string payload((char*)buffer, bufferSize);
    unsigned int end = payload.rfind("\r\n\r\n");
    if (end == std::string::npos) {
        return nullptr;
    } else {
        bytesParsed = end + 4;
    }

    payload.resize(end);
    Tokenizer tokr(payload, '\n');
    auto itr = tokr.begin();

    Tokenizer reqLineTokr(*itr, ' ');
    auto rlItr = reqLineTokr.begin();
    request->d_method = stringToMethod(*rlItr);
    ++rlItr;
    request->d_path = *rlItr;
    ++rlItr;
    unsigned int versionEnd = (*rlItr).find('\r');
    std::string versionStr = (*rlItr).substr(0, versionEnd);
    request->d_version = HttpVersion::fromString(versionStr);

    for (++itr; itr != tokr.end(); ++itr) {
        unsigned int split = (*itr).find(':');
        if (split == std::string::npos) {
            Log::Warn("Received bad request header: ", *itr);
        } else {
            std::string name = (*itr).substr(0, split);
            unsigned int valueEnd = (*itr).rfind('\r');
            std::string value = (*itr).substr(split + 2, valueEnd - split - 2);
            if (name == "Upgrade" && value == "h2c") {
                request->d_canUpgrade = true;
            }
            request->d_headers.push_back({name, value});
        }
    }

    return request;
}

uint8_t *HttpRequest::encode(uint32_t& length) const
{
    static const std::string VERSION = "HTTP/1.1";
    const std::string method = methodToString(d_method);
    length = method.length() + 1 + d_path.length() + 1 + VERSION.length() + 2;

    uint32_t contentLength = d_body.length();
    length += contentLength;

    std::vector<HttpHeaderField_t> headers(d_headers);
    if (contentLength > 0) {
        headers.push_back({HttpHeader::ContentLength, std::to_string(contentLength)});
    }

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
    memcpy(encoding + offset, method.data(), method.length());
    offset += method.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, d_path.data(), d_path.length());
    offset += d_path.length();
    encoding[offset++] = ' ';
    memcpy(encoding + offset, VERSION.data(), VERSION.length());
    offset += VERSION.length();
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

    memcpy(encoding + offset, d_body.data(), d_body.length());
    offset += contentLength;

    return encoding;
}

}
