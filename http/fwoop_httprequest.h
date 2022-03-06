#pragma once

#include <fwoop_httpheader.h>

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
    enum class Version {
        Undefined,
        Http1_0,
        Http1_1,
    };

  private:
    Method                         d_method;
    std::string                    d_path;
    Version                        d_version;
    std::vector<HttpHeaderField_t> d_headers;
    bool                           d_canUpgrade;

    static Method stringToMethod(const std::string& methodStr);
    static Version stringToVersion(const std::string& versionStr);

  public:
    static std::shared_ptr<HttpRequest> parse(uint8_t *buffer, uint32_t bufferSize, uint32_t& bytesParsed);

    explicit HttpRequest();

    Method getMethod() const;
    const std::string& getPath() const;
    Version getVersion() const;
    const std::vector<HttpHeaderField_t>& getHeaders() const;
    bool canUpgrade() const;
};

std::ostream& operator<<(std::ostream& os, HttpRequest::Method method);
std::ostream& operator<<(std::ostream& os, HttpRequest::Version version);
std::ostream& operator<<(std::ostream& os, const HttpRequest& request);

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
HttpRequest::Version HttpRequest::getVersion() const
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
