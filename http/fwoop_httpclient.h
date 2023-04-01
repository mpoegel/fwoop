#pragma once

#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>

#include <string>
#include <system_error>

namespace fwoop {

class HttpClient {
  private:
    std::string d_host;
    int d_port;
    int d_conn;

    void reset();

  public:
    HttpClient(const std::string &host, int port = 80);
    ~HttpClient();

    std::error_code makeReqest(const HttpRequest &request, std::shared_ptr<HttpResponse> &response);
};

} // namespace fwoop
