#include <fwoop_httprequest.h>

#include <fwoop_log.h>
#include <fwoop_tokenizer.h>

namespace fwoop {

std::ostream& operator<<(std::ostream& os, HttpRequest::Method method)
{
    switch (method) {
        case HttpRequest::Method::Get: os << "GET"; break;
        case HttpRequest::Method::Post: os << "POST"; break;
        case HttpRequest::Method::Put: os << "PUT"; break;
        case HttpRequest::Method::Delete: os << "DELETE"; break;
        case HttpRequest::Method::Undefined: os << "Undefined"; break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, HttpRequest::Version version)
{
    switch (version) {
        case HttpRequest::Version::Http1_0: os << "HTTP/1.0"; break;
        case HttpRequest::Version::Http1_1: os << "HTTP/1.1"; break;
        case HttpRequest::Version::Undefined: os << "Undefined"; break;
    }
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
, d_version(Version::Undefined)
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

HttpRequest::Version HttpRequest::stringToVersion(const std::string& versionStr)
{
    if (versionStr == "HTTP/1.0") return Version::Http1_0;
    if (versionStr == "HTTP/1.1") return Version::Http1_1;
    return Version::Undefined;
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
    request->d_version = stringToVersion(versionStr);

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

}
