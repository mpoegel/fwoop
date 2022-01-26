#pragma once

namespace fwoop {

class HttpServer {
  private:
    int d_port;
    int d_serverFd;

  public:
    explicit HttpServer(int port);
    ~HttpServer();

    int serve();
};

}