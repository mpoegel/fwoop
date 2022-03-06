#pragma once

#include <fwoop_log.h>

#include <cstring>
#include <iostream>
#include <system_error>

#include <sys/poll.h>
#include <unistd.h>

namespace fwoop {

class SocketIO {

  public:
    template<typename T>
    static std::error_code read(int fd, T *buffer, unsigned int bufferSize, unsigned int& bytesRead);

    template<typename T>
    static int write(int fd, T *out, unsigned int outLen);

};

template<typename T>
std::error_code SocketIO::read(int fd, T *buffer, unsigned int bufferSize, unsigned int& bytesRead)
{
    memset(buffer, 0, bufferSize);
    bytesRead = 0;

    struct pollfd pfd[1];
    pfd[0].fd = fd;
    pfd[0].events = POLLIN;

    int rc = poll(pfd, 1, 1000);
    if (rc == 0) {
        Log::Info("poll timed out");
        return std::error_code(errno, std::system_category());
    } else if (rc < 0) {
        std::cerr << "poll failed, errno=" << errno << ": " << std::strerror(errno) << '\n';
        // Log::Error("polled failed, errno=%d: %s", errno, std::strerror(errno));
        return std::error_code(errno, std::system_category());
    }

    if (pfd[0].revents & POLLIN) {
        rc = ::read(fd, buffer, bufferSize);
        if (0 == rc) {
            Log::Info("client closed the connection");
            return std::error_code(errno, std::system_category());
        } else if (rc < 0) {
            std::cerr << "read error, errno=" << errno << ": " << std::strerror(errno) << '\n';
            return std::error_code(errno, std::system_category());
        } else {
            bytesRead = rc;
        }
    }

    return std::error_code();
}

template<typename T>
int SocketIO::write(int fd, T *out, unsigned int outLen)
{
    int rc = 0;
    int bytesWritten = 0;
    while (bytesWritten < outLen) {
        rc = ::write(fd, out + bytesWritten, outLen - bytesWritten);
        if (rc < 0) {
            std::cerr << "write failed, errno=" << errno << ": " << std::strerror(errno) << '\n';
            return -1;
        }
        bytesWritten += rc;
    }
    return 0;
}

}
