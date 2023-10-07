#include <cstdint>
#include <fwoop_httpconnhandler.h>

#include <fwoop_log.h>
#include <fwoop_socketio.h>

#include <signal.h>
#include <system_error>

namespace fwoop {

HttpConnHandler::HttpConnHandler(const ReaderPtr_t &reader, const WriterPtr_t &writer, HttpRequestCallback *callback)
    : d_reader(reader), d_writer(writer), d_callback(callback)
{
}

HttpConnHandler::~HttpConnHandler() {}

HttpConnHandler::HttpConnHandler(HttpConnHandler &&rhs)
    : d_reader(rhs.d_reader), d_writer(rhs.d_writer), d_callback(rhs.d_callback)
{
}

void HttpConnHandler::operator()()
{
    signal(SIGPIPE, SIG_IGN);

    Log::Debug("received http/1.1 connection");
    constexpr unsigned int bufferSize = 2048;
    uint8_t buffer[bufferSize];
    unsigned int bytesRead;
    std::error_code ec = d_reader->read(buffer, bufferSize, bytesRead);
    if (ec) {
        Log::Error("socket read failed", ec);
    }

    unsigned int bytesParsed = 0;
    std::shared_ptr<HttpRequest> request = HttpRequest::parse(buffer, bytesRead, bytesParsed);
    if (!request) {
        Log::Error("did not receive full http request");
    }

    Log::Debug("Recieved request: ", *request);
    HttpResponse response;
    d_callback->onRequest(request, response);

    Log::Debug("Sending response: ", response);
    uint32_t length;
    uint32_t bytesWritten;
    uint8_t *encResp = response.encode(length);
    ec = d_writer->write(encResp, length, bytesWritten);
    delete[] encResp;
    if (ec) {
        Log::Error("socket write failed, ec=", ec);
    }

    d_callback->afterResponse(request, d_writer);
    Log::Debug("done");
    d_reader->close();
    d_writer->close();
}

} // namespace fwoop
