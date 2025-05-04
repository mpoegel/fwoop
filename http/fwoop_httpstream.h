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

    uint32_t id() const;
    int fd() const;
};

inline
uint32_t HttpStream::id() const
{
    return d_id;
}

inline
int HttpStream::fd() const
{
    return d_fd;
}

} // namespace fwoop
