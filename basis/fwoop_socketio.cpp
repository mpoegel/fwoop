#include <arpa/inet.h>
#include <cstdint>
#include <fwoop_array.h>
#include <fwoop_dnsquery.h>
#include <fwoop_socketio.h>
#include <memory>
#include <system_error>

namespace fwoop {

Socket::Socket(int fd) : d_fd(fd) {}

Socket::~Socket() {}

Socket::Socket(const Socket &rhs) : d_fd(rhs.d_fd) {}

SocketFactory::SocketFactory(const std::string &hostname, uint16_t port) : d_hostname(hostname), d_port(port) {}

SocketBasePtr_t SocketFactory::connect()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        Log::Error("failed to create socket, errno=", errno);
        return nullptr;
    }

    const int opt = 1;
    if (0 != setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        Log::Error("failed to setsockopt, errno=", errno);
        return nullptr;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(d_port);

    auto record = DNS::Query::GetRecord(d_hostname);
    if (record == nullptr) {
        Log::Error("hostname ", d_hostname, " not found");
        return nullptr;
    }
    if (inet_pton(AF_INET, record->IP().c_str(), &serv_addr.sin_addr) <= 0) {
        Log::Error("invalid address or address not supported");
        return nullptr;
    }

    int rc = ::connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (0 != rc) {
        // connect failed
        return nullptr;
    }

    return std::make_shared<Socket>(fd);
}

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
        rc = ::read(d_fd, *arr, arr.maxSize());
        if (0 == rc) {
            // peer closed the connection
            return std::error_code(errno, std::system_category());
        } else if (rc < 0) {
            // read error
            return std::error_code(errno, std::system_category());
        } else {
            arr.enlarge(rc);
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
