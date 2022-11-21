#pragma once

#include <fwoop_httpserverevent.h>
#include <fwoop_httprequest.h>
#include <fwoop_httpresponse.h>
#include <fwoop_httpversion.h>

#include <functional>
#include <string>
#include <unordered_map>

namespace fwoop {

typedef std::function<void(const HttpRequest&, HttpResponse&)> HttpHandlerFunc_t;
typedef std::function<void(const HttpRequest&, HttpServerEvent&)> HttpServerEventHandlerFunc_t;

class HttpServer {
  private:
    int d_port;
    int d_serverFd;
    HttpVersion::Value d_version;
    std::unordered_map<std::string, HttpHandlerFunc_t> d_routeMap;
    std::unordered_map<std::string, HttpServerEventHandlerFunc_t> d_serverEventMap;
    bool d_isActive;

    int parsePayloadBody(uint8_t *buffer, unsigned bufferSize, unsigned int& bytesParsed) const;
    int handleHttp1Connection(int clientFd) const;
    int handleHttp2Connection(int clientFd) const;

  public:
    explicit HttpServer(int port, HttpVersion::Value version);
    ~HttpServer();

    int serve();
    void stop();

    void addRoute(const std::string& route, HttpHandlerFunc_t func);
    void addStaticRoute(const std::string& route, const std::string& fileName);
    void addServerEventRoute(const std::string& route, HttpServerEventHandlerFunc_t func);
};

inline
void HttpServer::stop()
{
    d_isActive = false;
}

}
