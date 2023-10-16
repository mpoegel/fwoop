#include <cstdint>
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

    auto sockFactory = fwoop::SecureSocketFactory("botan.randombit.net", 443);
    sockFactory.addTrustStore("/etc/ssl/certs");
    auto sock = sockFactory.connect();
    ::sleep(1);
    if (sock) {
        fwoop::Log::Info("connected");
        uint8_t buf[16384];
        uint32_t bytesRead = 0;
        uint8_t msg[] = "GET / HTTP/1.1\r\n"
                        "Host: botan.randombit.net\r\n\r\n";
        uint32_t bytesWritten = 0;
        auto ec = sock->write(msg, sizeof(msg), bytesWritten);
        if (ec) {
            fwoop::Log::Error("secure write failed: ", ec.message());
        }
        ec = sock->read(buf, sizeof(buf), bytesRead);
        if (ec) {
            fwoop::Log::Error("secure read failed: ", ec.message());
        }
        fwoop::Log::Info("DATA: ", std::string(buf, buf + bytesRead));
    }

    fwoop::Log::Info("done");
    return 0;
}
