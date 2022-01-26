#include <fwoop_httpserver.h>
#include <fwoop_tokenizer.h>

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

    constexpr int bufferSize = 1024;
    char buffer[bufferSize];

    struct pollfd pfd[1];
    pfd[0].fd = clientFd;
    pfd[0].events = POLLIN;

    int rc = poll(pfd, 1, 1000);
    if (rc == 0) {
        std::cerr << "poll timed out\n";
        close(clientFd);
        return -1;
    } else if (rc < 0) {
        std::cerr << "poll failed, errno=" << errno << '\n';
        close(clientFd);
        return -1;
    }

    if (pfd[0].revents & POLLIN) {
        rc = read(clientFd, buffer, bufferSize);
        if (0 == rc) {
            std::cerr << "client closed the connection\n";
        } else if (rc < 0) {
            std::cerr << "read error, errno=" << errno << '\n';
        }
    }

    Tokenizer tokr(std::string(buffer), '\n');
    for (auto itr = tokr.begin(); itr != tokr.end(); ++itr) {
        std::cout << "< " << *itr << '\n';
    }

    close(clientFd);

    return 0;
}

}
