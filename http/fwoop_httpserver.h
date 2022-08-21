#pragma once

#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>

#include <functional>
#include <string>
#include <unordered_map>

namespace fwoop {

enum HttpVersion {
    HTTP11 = 0,
    HTTP2 = 1,
    HTTP3 = 2,
};

typedef std::function<void(const HttpRequest&, HttpResponse&)> HttpHandlerFunc_t;

class HttpServer {
  private:
    int d_port;
    int d_serverFd;
    HttpVersion d_version;
    std::unordered_map<std::string, HttpHandlerFunc_t> d_routeMap;

    int parsePayloadBody(uint8_t *buffer, unsigned bufferSize, unsigned int& bytesParsed) const;
    int handleHttp1Connection(int clientFd) const;
    int handleHttp2Connection(int clientFd) const;

  public:
    explicit HttpServer(int port, HttpVersion version);
    ~HttpServer();

    int serve();

    void addRoute(const std::string& route, HttpHandlerFunc_t func);
    void addStaticRoute(const std::string& route, const std::string& fileName);
};

}
