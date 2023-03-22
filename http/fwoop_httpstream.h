#pragma once

#include <stdint.h>

namespace fwoop {

class HttpStream {
  private:
    static uint32_t s_nextID;

    uint32_t d_id;
    int d_fd;

  public:
    explicit HttpStream(int clientFd);

    int open();
    int close();
};

} // namespace fwoop
