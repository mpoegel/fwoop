#pragma once

#include <fwoop_socketio.h>

#include <string>

namespace fwoop {

class HttpServerEvent {
  private:
    WriterPtr_t d_writer;

  public:
    explicit HttpServerEvent(WriterPtr_t writer);
    ~HttpServerEvent();

    bool pushEvent(const std::string &event, const std::string &data);
};

} // namespace fwoop
