#include <fwoop_socketio.h>
#include <system_error>

namespace fwoop {

Socket::Socket(int fd) : d_fd(fd) {}

Socket::~Socket() {}

std::error_code Socket::read(uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesRead)
{
    memset(buffer, 0, bufferSize);
    bytesRead = 0;

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
        rc = ::read(d_fd, buffer, bufferSize);
        if (0 == rc) {
            // peer closed the connection
            return std::error_code(errno, std::system_category());
        } else if (rc < 0) {
            // read error
            return std::error_code(errno, std::system_category());
        } else {
            bytesRead = rc;
        }
    }
    return std::error_code();
}

std::error_code Socket::write(const uint8_t *out, uint32_t outLen, uint32_t &bytesWritten)
{
    int rc = 0;
    bytesWritten = 0;
    while (bytesWritten < outLen) {
        rc = ::write(d_fd, out + bytesWritten, outLen - bytesWritten);
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
