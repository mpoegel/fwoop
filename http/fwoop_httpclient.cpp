#include "fwoop_httpresponse.h"
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

HttpClient::HttpClient(const std::string &host, int port) : d_host(host), d_port(port), d_conn(-1) {}

HttpClient::~HttpClient()
{
    if (d_conn > 0) {
        close(d_conn);
    }
}

void HttpClient::reset()
{
    close(d_conn);
    d_conn = -1;
}

std::error_code HttpClient::makeReqest(const HttpRequest &request, std::shared_ptr<HttpResponse> &response)
{
    if (d_conn < 0) {
        d_conn = socket(AF_INET, SOCK_STREAM, 0);
        if (d_conn < 0) {
            Log::Error("failed to create socket, errno=", errno);
            return std::error_code(static_cast<int>(HttpErrc::SocketError), HttpClientError);
        }

        const int opt = 1;
        if (0 != setsockopt(d_conn, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            Log::Error("failed to setsockopt, errno=", errno);
            reset();
            return std::error_code(static_cast<int>(HttpErrc::SocketError), HttpClientError);
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(d_port);

        auto record = DNS::Query::getRecord(d_host);
        if (record == nullptr) {
            Log::Error("hostname ", d_host, " not found");
            return std::error_code(static_cast<int>(HttpErrc::HostNotFound), HttpClientError);
        }
        if (inet_pton(AF_INET, record->IP().c_str(), &serv_addr.sin_addr) <= 0) {
            Log::Error("invalid address or address not supported");
            reset();
            return std::error_code(static_cast<int>(HttpErrc::HostNotFound), HttpClientError);
        }

        int rc = connect(d_conn, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (0 != rc) {
            Log::Error("connection failed");
            reset();
            return std::error_code(static_cast<int>(HttpErrc::ConnectFailed), HttpClientError);
        }
        Log::Debug("connected to ", d_host);
    }

    uint32_t length;
    uint8_t *encReq = request.encode(length);

    int rc = SocketIO::write(d_conn, encReq, length);
    delete[] encReq;
    if (0 != rc) {
        reset();
        return std::error_code(static_cast<int>(HttpErrc::WriteFailed), HttpClientError);
    }
    ::usleep(5 * 1000);

    Log::Debug("wrote ", length, " bytes");

    constexpr unsigned int bufferSize = 32768;
    uint8_t buffer[bufferSize];
    unsigned int bytesRead = 0;
    auto br = HttpResponse::BuildResult::Incomplete;
    response = std::make_shared<HttpResponse>();

    while (br == HttpResponse::BuildResult::Incomplete) {
        auto ec = SocketIO::read(d_conn, buffer, bufferSize, bytesRead);
        if (ec && ec.value() != ETIMEDOUT) {
            reset();
            return std::error_code(static_cast<int>(HttpErrc::ReadFailed), HttpClientError);
        }

        Log::Debug("read ", bytesRead, " bytes");
        unsigned int bytesParsed;
        br = response->build(buffer, bytesRead, bytesParsed);
        Log::Debug("parsed ", bytesParsed, " bytes");
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
    case HttpClient::HttpErrc::WriteFailed:
        return "write failed";
    case HttpClient::HttpErrc::ReadFailed:
        return "read failed";
    default:
        return "unknown http client error";
    }
}

} // namespace fwoop
