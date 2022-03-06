#pragma once

#include <string>

namespace fwoop {

class HttpServer {
  private:
    int d_port;
    int d_serverFd;

    int parsePayloadBody(uint8_t *buffer, unsigned bufferSize, unsigned int& bytesParsed) const;
    int handleConnection(int clientFd) const;

  public:
    explicit HttpServer(int port);
    ~HttpServer();

    int serve();
};

}
