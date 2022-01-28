#pragma once

#include <string>

namespace fwoop {

class HttpServer {
  private:
    int d_port;
    int d_serverFd;

    int read(int fd, uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesRead);

    int parsePayloadBody(uint8_t *buffer, unsigned bufferSize, unsigned int& bytesParsed) const;

  public:
    explicit HttpServer(int port);
    ~HttpServer();

    int serve();
};

}