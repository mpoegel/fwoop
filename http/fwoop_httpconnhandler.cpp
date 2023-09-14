#include <fwoop_httpconnhandler.h>

#include <fwoop_log.h>
#include <fwoop_socketio.h>

namespace fwoop {

HttpConnHandler::HttpConnHandler(int fd, const std::unordered_map<std::string, HttpHandlerFunc_t> &routeMap,
                                 const std::unordered_map<std::string, HttpServerEventHandlerFunc_t> &serverEventMap)
    : d_fd(fd), d_routeMap(routeMap), d_serverEventMap(serverEventMap)
{
}

HttpConnHandler::~HttpConnHandler()
{
    if (d_fd > 0) {
        close(d_fd);
        d_fd = -1;
    }
}

HttpConnHandler::HttpConnHandler(HttpConnHandler &&rhs)
    : d_fd(rhs.d_fd), d_routeMap(rhs.d_routeMap), d_serverEventMap(rhs.d_serverEventMap)
{
    rhs.d_fd = -1;
}

void HttpConnHandler::operator()()
{
    Log::Debug("received http/1.1 connection");
    constexpr unsigned int bufferSize = 2048;
    uint8_t buffer[bufferSize];
    unsigned int bytesRead;
    std::error_code ec = SocketIO::read(d_fd, buffer, bufferSize, bytesRead);
    if (ec) {
        Log::Error("socket read failed", ec);
    }

    unsigned int bytesParsed = 0;
    int rc;
    std::shared_ptr<HttpRequest> request = HttpRequest::parse(buffer, bytesRead, bytesParsed);
    if (!request) {
        Log::Error("did not receive full http request");
    }

    Log::Debug("Recieved request: ", *request);

    auto routeFunc = d_routeMap.find(request->getPath());
    auto serverEventFunc = d_serverEventMap.find(request->getPath());
    HttpResponse response;
    if (routeFunc != d_routeMap.end()) {
        routeFunc->second(*request, response);
    } else if (serverEventFunc != d_serverEventMap.end()) {
        response.addHeader(HttpHeader::ContentType, "text/event-stream");
        response.addHeader(HttpHeader::CacheControl, "no-store");
        response.setStatus("200 OK");
    } else {
        response.setStatus("404 Not Found");
    }

    Log::Debug("Sending response: ", response);
    uint32_t length;
    uint8_t *encResp = response.encode(length);
    rc = SocketIO::write(d_fd, encResp, length);
    delete[] encResp;
    if (0 != rc) {
        Log::Error("socket write failed, ec=", ec);
    }

    if (serverEventFunc != d_serverEventMap.end()) {
        HttpServerEvent serverEvent(d_fd);
        serverEventFunc->second(*request, serverEvent);
    }

    Log::Debug("done");
}

} // namespace fwoop
