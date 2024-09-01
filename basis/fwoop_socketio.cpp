#include "fwoop_array.h"
#include <fwoop_socketio.h>
#include <system_error>

namespace fwoop {

Socket::Socket(int fd) : d_fd(fd) {}

Socket::~Socket() {}

Socket::Socket(const Socket &rhs) : d_fd(rhs.d_fd) {}

std::error_code Socket::read(Array &arr)
{
    struct pollfd pfd[1];
    pfd[0].fd = d_fd;
    pfd[0].events = POLLIN;

    int rc = poll(pfd, 1, 1000);
    if (rc == 0) {
        // poll timed out
        return std::error_code(errno, std::system_category());
    } else if (rc < 0) {
        // poll failed
        return std::error_code(errno, std::system_category());
    }

    if (pfd[0].revents & POLLIN) {
        rc = ::read(d_fd, *arr, arr.size());
        if (0 == rc) {
            // peer closed the connection
            return std::error_code(errno, std::system_category());
        } else if (rc < 0) {
            // read error
            return std::error_code(errno, std::system_category());
        } else {
            arr.shrink(rc);
        }
    }
    return std::error_code();
}

std::error_code Socket::write(const Array &arr, uint32_t &bytesWritten)
{
    int rc = 0;
    bytesWritten = 0;
    while (bytesWritten < arr.size()) {
        rc = ::write(d_fd, *arr + bytesWritten, arr.size() - bytesWritten);
        if (rc < 0) {
            // write failed
            return std::error_code(errno, std::system_category());
        }
        bytesWritten += rc;
    }
    return std::error_code();
}

void Socket::close()
{
    if (d_fd >= 0) {
        ::close(d_fd);
        d_fd = -1;
    }
}

} // namespace fwoop
