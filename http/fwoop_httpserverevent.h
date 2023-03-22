#pragma once

#include <string>

namespace fwoop {

class HttpServerEvent {
  private:
    int d_fd;

  public:
    explicit HttpServerEvent(int fd);
    ~HttpServerEvent();

    bool pushEvent(const std::string &event, const std::string &data);
};

} // namespace fwoop
