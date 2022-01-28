#include <fwoop_httpserver.h>
#include <fwoop_tokenizer.h>
#include <fwoop_httpframe.h>

#include <cstring>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace fwoop {

HttpServer::HttpServer(int port)
: d_port(port)
, d_serverFd(-1)
{}

HttpServer::~HttpServer()
{
    if (d_serverFd >= 0) {
        close(d_serverFd);
    }
}

int HttpServer::serve()
{
    d_serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (d_serverFd < 0) {
        std::cerr << "failed to create socket, errno" << errno << '\n';
        return -1;
    }

    const int opt = 1;
    if (0 != setsockopt(d_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "failed to setsockopt, errno=" << errno << '\n';
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(d_port);

    if (0 != bind(d_serverFd, (struct sockaddr*)&addr, sizeof(addr))) {
        std::cerr << "failed to bind, errno=" << errno << '\n';
        return -1;
    }

    const int maxQueued = 1;
    if (0 != listen(d_serverFd, maxQueued)) {
        std::cerr << "failed to listen, errno" << errno << '\n';
        return -1;
    }

    struct sockaddr_in clientAddr;
    socklen_t addrLen;
    int clientFd = accept4(d_serverFd, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen, SOCK_NONBLOCK);
    if (clientFd < 0) {
        std::cerr << "failed to accept, errno" << errno << '\n';
        return -1;
    }

    constexpr unsigned int bufferSize = 1024;
    uint8_t buffer[bufferSize];
    unsigned int bytesRead = 0;
    if (0 != read(clientFd, buffer, bufferSize, bytesRead)) {
        close(clientFd);
    }
    unsigned int bytesParsed = 0;
    int rc;
    rc = parsePayloadBody(buffer, bytesRead, bytesParsed);

    std::string resp = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: h2c\r\n\r\n";
    std::cout << "> " << resp;
    int bytesWritten = 0;
    while (bytesWritten < resp.length()) {
        rc = write(clientFd, resp.c_str(), resp.length());
        if (rc < 0) {
            std::cerr << "write failed, errno=" << errno << '\n';
            return -1;
        }
        bytesWritten += rc;
    }

    rc = read(clientFd, buffer, bufferSize, bytesRead);
    if (0 != rc) {
        close(clientFd);
        return -1;
    }
    rc = parsePayloadBody(buffer, bytesRead, bytesParsed);

    unsigned int offset = 0;
    if (bytesRead > bytesParsed) {
        memcpy(buffer, buffer + bytesParsed, bytesRead - bytesParsed);
        offset = (bytesRead - bytesParsed);
    }

    if (0 != read(clientFd, buffer + offset, bufferSize - offset, bytesRead)) {
        close(clientFd);
    }
    auto frame = HttpFrame::parse(buffer, bytesRead + offset, bytesParsed);
    if (frame) {
        std::cout << "frame: len=" << frame->length() << ", type=" << HttpFrame::typeToString(frame->type()) << '\n';
    } else {
        std::cerr << "no frame\n";
    }

    close(clientFd);

    return 0;
}

int HttpServer::read(int fd, uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesRead)
{
    memset(buffer, 0, bufferSize);
    bytesRead = 0;

    struct pollfd pfd[1];
    pfd[0].fd = fd;
    pfd[0].events = POLLIN;

    int rc = poll(pfd, 1, 1000);
    if (rc == 0) {
        std::cerr << "poll timed out\n";
        return -1;
    } else if (rc < 0) {
        std::cerr << "poll failed, errno=" << errno << '\n';
        return -1;
    }

    if (pfd[0].revents & POLLIN) {
        rc = ::read(fd, buffer, bufferSize);
        if (0 == rc) {
            std::cerr << "client closed the connection\n";
            return -1;
        } else if (rc < 0) {
            std::cerr << "read error, errno=" << errno << '\n';
            return -1;
        } else {
            bytesRead = rc;
        }
    }

    return 0;
}

int HttpServer::parsePayloadBody(uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesParsed) const
{
    bytesParsed = 0;
    std::string payload((char*)buffer, bufferSize);
    unsigned int end = payload.rfind("\r\n\r\n");
    if (end == std::string::npos) {
        bytesParsed = bufferSize;
    } else {
        bytesParsed = end + 4;
    }

    payload.resize(bytesParsed);
    Tokenizer tokr(payload, '\n');
    for (auto itr = tokr.begin(); itr != tokr.end(); ++itr) {
        std::cout << "< " << *itr << '\n';
    }

    return 0;
}

}
