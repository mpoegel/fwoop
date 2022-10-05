#pragma once

#include <fwoop_httpheader.h>
#include <fwoop_httpversion.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace fwoop {

class HttpRequest {
  public:
    enum class Method {
        Undefined,
        Get,
        Post,
        Put,
        Delete,
    };

  private:
    Method                         d_method;
    std::string                    d_path;
    HttpVersion::Value             d_version;
    std::vector<HttpHeaderField_t> d_headers;
    bool                           d_canUpgrade;
    std::string                    d_body;

    static Method stringToMethod(const std::string& methodStr);

  public:
    static std::shared_ptr<HttpRequest> parse(uint8_t *buffer, uint32_t bufferSize, uint32_t& bytesParsed);
    static std::string methodToString(const Method& method);

    explicit HttpRequest();

    void addHeader(const HttpHeader& name, const std::string& value);
    void addHeader(const HttpCustomHeader& name, const std::string& value);
    void setPath(const std::string& path);
    void setBody(const std::string& body);
    void setMethod(const Method& method);

    Method getMethod() const;
    const std::string& getPath() const;
    HttpVersion::Value getVersion() const;
    const std::vector<HttpHeaderField_t>& getHeaders() const;
    bool canUpgrade() const;
    uint8_t *encode(uint32_t& length) const;
};

std::ostream& operator<<(std::ostream& os, HttpRequest::Method method);
std::ostream& operator<<(std::ostream& os, const HttpRequest& request);

inline
void HttpRequest::addHeader(const HttpHeader& name, const std::string& value)
{
    d_headers.push_back({name, value});
}

inline
void HttpRequest::addHeader(const HttpCustomHeader& name, const std::string& value)
{
    d_headers.push_back({name, value});
}

inline
void HttpRequest::setPath(const std::string& path)
{
    d_path = path;
}

inline
void HttpRequest::setBody(const std::string& body)
{
    d_body = body;
}

inline
void HttpRequest::setMethod(const Method& method)
{
    d_method = method;
}

inline
HttpRequest::Method HttpRequest::getMethod() const
{
    return d_method;
}

inline
const std::string& HttpRequest::getPath() const
{
    return d_path;
}

inline
HttpVersion::Value HttpRequest::getVersion() const
{
    return d_version;
}

inline
const std::vector<HttpHeaderField_t>& HttpRequest::getHeaders() const
{
    return d_headers;
}

inline
bool HttpRequest::canUpgrade() const
{
    return d_canUpgrade;
}

}
