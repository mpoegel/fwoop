#include "fwoop_array.h"
#include <cstdint>
#include <cstring>
#include <iostream>

#include <fwoop_log.h>
#include <fwoop_securesocket.h>
#include <memory>
#include <thread>

int main(int argc, char *argv[])
{
    const std::string certFile("cert.pem");
    const std::string keyFile("key.pem");
    const std::string authFile("ca.pem");
    const std::string hostname("localhost");
    const uint16_t port = 9099;

    auto sockFactory = fwoop::SecureSocketFactory("httpbin.org", 443);
    sockFactory.addTrustStore("/etc/ssl/certs");
    auto sock = sockFactory.connect();
    ::sleep(1);
    if (sock) {
        fwoop::Log::Info("connected");
        uint32_t bytesRead = 0;
        uint8_t msg[] = "GET /get HTTP/1.1\r\n"
                        "Host: httpbin.org\r\n"
                        "User-Agent: fwoop/1\r\n"
                        "Accept: */*\r\n\r\n";
        uint32_t bytesWritten = 0;
        fwoop::Array arr(sizeof(msg));
        arr.append(msg, sizeof(msg) - 1);
        auto ec = sock->write(arr, bytesWritten);
        if (ec) {
            fwoop::Log::Error("secure write failed: ", ec.message());
            return 1;
        }
        const uint32_t bufSize = 16384;
        fwoop::Array buf(bufSize);
        do {
            buf.clear();
            ec = sock->read(buf);
            if (ec) {
                fwoop::Log::Error("secure read failed: ", ec.message());
                return 1;
            }
            fwoop::Log::Info("readBytes=", bytesRead, " DATA: ", buf.toString());
        } while (!ec && buf.size() > 0);
    }

    fwoop::Log::Info("done");
    return 0;
}
