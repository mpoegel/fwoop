#pragma once

#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_socketio.h>

#include <string>
#include <system_error>

namespace fwoop {

class HttpClient {
  public:
    enum HttpErrc {
        SocketError = 1,
        ConnectFailed,
        HostNotFound,
        WriteFailed,
        ReadFailed,
    };

  private:
    SocketFactoryBasePtr_t d_socketFactory;

    void reset();

  public:
    HttpClient(const std::string &host, int port = 80);
    HttpClient(const SocketFactoryBasePtr_t &factory);
    ~HttpClient();

    std::error_code makeReqest(const HttpRequest &request, std::shared_ptr<HttpResponse> &response);
};

struct HttpClientErrCategory : std::error_category {
    const char *name() const noexcept override;
    std::string message(int ev) const override;
};

} // namespace fwoop
