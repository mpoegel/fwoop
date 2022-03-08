#pragma once

#include <fwoop_httpheader.h>

#include <string>
#include <vector>

namespace fwoop {

class HttpResponse {
  private:
    std::vector<HttpHeaderField_t> d_headers;
    std::string                    d_status;

  public:
    explicit HttpResponse();

    void setStatus(const std::string& status);
    void addHeader(const HttpHeader& name, const std::string& value);
    void addHeader(const HttpCustomHeader& name, const std::string& value);

    uint8_t *encode(uint32_t& length) const;
};

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

}
