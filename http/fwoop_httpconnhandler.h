#pragma once

#include <fwoop_httpfunc.h>

namespace fwoop {

class HttpConnHandler {
  private:
    int d_fd;
    const std::unordered_map<std::string, HttpHandlerFunc_t> d_routeMap;
    const std::unordered_map<std::string, HttpServerEventHandlerFunc_t> d_serverEventMap;

  public:
    HttpConnHandler(int fd, const std::unordered_map<std::string, HttpHandlerFunc_t> &routeMap,
                    const std::unordered_map<std::string, HttpServerEventHandlerFunc_t> &serverEventMap);
    ~HttpConnHandler();
    HttpConnHandler(const HttpConnHandler &rhs) = delete;
    HttpConnHandler operator=(const HttpConnHandler &rhs) = delete;
    HttpConnHandler(HttpConnHandler &&rhs);

    void operator()();
};

} // namespace fwoop
