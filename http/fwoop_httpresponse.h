#pragma once

#include <fwoop_httpheader.h>

#include <ostream>
#include <string>
#include <vector>

namespace fwoop {

class HttpResponse {
  private:
    std::vector<HttpHeaderField_t> d_headers;
    std::string                    d_status;
    std::string                    d_body;
    std::string                    d_fileName;

  public:
    explicit HttpResponse();

    void setStatus(const std::string& status);
    void addHeader(const HttpHeader& name, const std::string& value);
    void addHeader(const HttpCustomHeader& name, const std::string& value);
    void setBody(const std::string& body);
    void streamFile(const std::string& fileName);

    uint8_t *encode(uint32_t& length) const;
    const std::vector<HttpHeaderField_t> getHeaders() const;
    const std::string& getStatus() const;
    const std::string& getBody() const;
    const std::string& getFile() const;
};

std::ostream& operator<<(std::ostream& os, const HttpResponse& response);

inline
void HttpResponse::setStatus(const std::string& status)
{
    d_status = status;
}

inline
void HttpResponse::addHeader(const HttpHeader& name, const std::string& value)
{
    d_headers.push_back({name, value});
}

inline
void HttpResponse::addHeader(const HttpCustomHeader& name, const std::string& value)
{
    d_headers.push_back({name, value});
}

inline
void HttpResponse::setBody(const std::string& body)
{
    d_body = body;
}

inline
void HttpResponse::streamFile(const std::string& fileName)
{
    d_fileName = fileName;
}

inline
const std::vector<HttpHeaderField_t> HttpResponse::getHeaders() const
{
    return d_headers;
}

inline
const std::string& HttpResponse::getStatus() const
{
    return d_status;
}

inline
const std::string& HttpResponse::getBody() const
{
    return d_body;
}

inline
const std::string& HttpResponse::getFile() const
{
    return d_fileName;
}

}