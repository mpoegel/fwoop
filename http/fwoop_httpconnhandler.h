#pragma once

#include <fwoop_httpfunc.h>
#include <fwoop_socketio.h>

namespace fwoop {

class HttpRequestCallback {
  public:
    ~HttpRequestCallback() {}
    virtual void onRequest(const std::shared_ptr<HttpRequest> &request, HttpResponse &response) = 0;
    virtual void afterResponse(const std::shared_ptr<HttpRequest> &req, WriterPtr_t writer) = 0;
};

class HttpConnHandler {
  private:
    ReaderPtr_t d_reader;
    WriterPtr_t d_writer;
    HttpRequestCallback *d_callback;

  public:
    HttpConnHandler(const ReaderPtr_t &reader, const WriterPtr_t &writer, HttpRequestCallback *callback);
    ~HttpConnHandler();
    HttpConnHandler(const HttpConnHandler &rhs) = delete;
    HttpConnHandler operator=(const HttpConnHandler &rhs) = delete;
    HttpConnHandler(HttpConnHandler &&rhs);

    void operator()();
};

} // namespace fwoop
