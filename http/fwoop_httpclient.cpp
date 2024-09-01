#include "fwoop_httpresponse.h"
#include <cstdint>
#include <fwoop_httpclient.h>

#include <fwoop_dnsquery.h>
#include <fwoop_log.h>
#include <fwoop_socketio.h>

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace fwoop {

const HttpClientErrCategory HttpClientError{};

HttpClient::HttpClient(const std::string &host, int port) : d_socketFactory(std::make_shared<SocketFactory>(host, port))
{
}

HttpClient::HttpClient(const SocketFactoryBasePtr_t &factory) : d_socketFactory(factory) {}

HttpClient::~HttpClient() {}

void HttpClient::reset() {}

std::error_code HttpClient::makeReqest(const HttpRequest &request, std::shared_ptr<HttpResponse> &response)
{
    // TODO close conn
    auto conn = d_socketFactory->connect();
    if (!conn) {
        return std::error_code(static_cast<int>(HttpErrc::HostNotFound), HttpClientError);
    }

    uint32_t length;
    uint8_t *encReq = request.encode(length);
    Array arr(length);
    arr.append(encReq, length);
    delete[] encReq;

    uint32_t bytesWritten;
    auto ec = conn->write(arr, bytesWritten);
    if (ec) {
        reset();
        return std::error_code(static_cast<int>(HttpErrc::WriteFailed), HttpClientError);
    }
    ::usleep(5 * 1000);

    Log::Debug("wrote ", bytesWritten, " bytes");

    constexpr unsigned int bufferSize = 32768;
    Array buf(bufferSize);
    unsigned int totalRead = 0;
    unsigned int totalParsed = 0;
    auto br = HttpResponse::BuildResult::Incomplete;
    response = std::make_shared<HttpResponse>();

    while (br == HttpResponse::BuildResult::Incomplete) {
        ec = conn->read(buf);
        if (ec && ec.value() != ETIMEDOUT) {
            reset();
            return std::error_code(static_cast<int>(HttpErrc::ReadFailed), HttpClientError);
        }

        totalRead += buf.size();
        unsigned int bytesParsed = 0;
        br = response->build(*buf, buf.size(), bytesParsed);
        totalParsed += bytesParsed;
        Log::Debug("totalRead=", totalRead, " totalParsed=", totalParsed);
    }

    return std::error_code();
}

const char *HttpClientErrCategory::name() const noexcept { return "HttpClient"; }

std::string HttpClientErrCategory::message(int ev) const
{
    switch (static_cast<HttpClient::HttpErrc>(ev)) {
    case HttpClient::HttpErrc::SocketError:
        return "socket error";
    case HttpClient::HttpErrc::ConnectFailed:
        return "connect failed";
    case HttpClient::HttpErrc::HostNotFound:
        return "host not found";
    case HttpClient::HttpErrc::WriteFailed:
        return "write failed";
    case HttpClient::HttpErrc::ReadFailed:
        return "read failed";
    default:
        return "unknown http client error";
    }
}

} // namespace fwoop
