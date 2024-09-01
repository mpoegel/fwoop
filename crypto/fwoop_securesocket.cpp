#include <arpa/inet.h>
#include <botan/auto_rng.h>
#include <botan/tls_client.h>
#include <botan/tls_server_info.h>
#include <botan/tls_session_manager_memory.h>
#include <cstdint>
#include <cstring>
#include <fwoop_dnsquery.h>
#include <fwoop_securesocket.h>
#include <fwoop_socketio.h>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace fwoop {

SecureSocket::SecureSocket(int fd, const SecureSocketConfig &config) : d_fd(fd)
{
    d_callbacks = std::make_shared<SecureCallbacks>(d_fd);

    auto policy = std::make_shared<SecureSocketPolicy>();

    Botan::TLS::Protocol_Version version = Botan::TLS::Protocol_Version::TLS_V12;
    auto info = Botan::TLS::Server_Information(config.hostname, config.port);

    auto creds = std::make_shared<tls::ClientCredentials>();
    creds->loadTrustedStoreFromFile("/etc/ssl/certs");
    auto rng = std::make_shared<Botan::AutoSeeded_RNG>();
    auto mgr = std::make_shared<Botan::TLS::Session_Manager_In_Memory>(rng);

    d_client = std::make_shared<Botan::TLS::Client>(d_callbacks, mgr, creds, policy, rng, info, version);
}

SecureSocket::~SecureSocket() {}

std::error_code SecureSocket::handshake()
{
    uint8_t buf[16384];
    uint32_t bytesRead = 0;
    std::error_code ec;
    while (!d_client->is_active() && !ec) {
        ec = read(buf, sizeof(buf), bytesRead);
    }
    return ec;
}

std::error_code SecureSocket::read(uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesRead)
{
    bytesRead = 0;

    // return early if there's already pending data
    d_callbacks->readWaiting(buffer, bufferSize, bytesRead);
    if (bytesRead > 0) {
        return std::error_code();
    }

    size_t numMoreBytes = bufferSize;
    struct pollfd pfd[1];
    pfd[0].fd = d_fd;
    pfd[0].events = POLLIN;

    while (numMoreBytes > 0 && bytesRead < bufferSize) {
        int rc = poll(pfd, 1, 1000);
        if (rc == 0) {
            // poll timed out
            return std::error_code(errno, std::system_category());
        } else if (rc < 0) {
            // poll failed
            return std::error_code(errno, std::system_category());
        }

        if (pfd[0].revents & POLLIN) {
            rc = ::read(d_fd, buffer, numMoreBytes);
            if (0 == rc) {
                // peer closed the connection
                return std::error_code(errno, std::system_category());
            } else if (rc < 0) {
                // read error
                return std::error_code(errno, std::system_category());
            } else {
                bytesRead = rc;
            }
        }

        Log::Debug("tls read ", bytesRead, " bytes");
        // decrypt the incoming data
        numMoreBytes = d_client->received_data(buffer, bytesRead);
        Log::Debug("need ", numMoreBytes, " more bytes to complete TLS record");
    }

    if (bytesRead == bufferSize) {
        Log::Error("read buffer full!");
        // TODO return error
    }

    // reset the buffer
    memset(buffer, 0, bufferSize);
    bytesRead = 0;
    // copy the decrypted data into the buffer
    d_callbacks->readWaiting(buffer, bufferSize, bytesRead);
    return std::error_code();
}

std::error_code SecureSocket::write(const uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesWritten)
{
    if (d_client->is_closed_for_writing()) {
        Log::Error("cannot write at this time");
        return std::error_code(errno, std::system_category());
    }
    if (!d_client->is_active()) {
        handshake();
    }
    d_client->send(buffer, bufferSize);
    bytesWritten = bufferSize;
    return std::error_code();
}

void SecureSocket::close()
{
    if (d_client->is_active()) {
        d_client->close();
    }
    if (d_fd > 0) {
        ::close(d_fd);
        d_fd = -1;
    }
}

SecureSocketFactory::SecureSocketFactory(const std::string &hostname, uint16_t port)
    : d_hostname(hostname), d_port(port), d_creds(std::make_shared<tls::ClientCredentials>()),
      d_rng(std::make_shared<Botan::AutoSeeded_RNG>()),
      d_sessionMgr(std::make_shared<Botan::TLS::Session_Manager_In_Memory>(d_rng))
{
}

SecureSocketFactory::~SecureSocketFactory() {}

SocketBasePtr_t SecureSocketFactory::connect()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        Log::Error("failed to create socket, errno=", errno);
        return nullptr;
    }

    const int opt = 1;
    if (0 != setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        Log::Error("failed to setsockopt, errno=", errno);
        return nullptr;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(d_port);

    auto record = DNS::Query::GetRecord(d_hostname);
    if (record == nullptr) {
        Log::Error("hostname ", d_hostname, " not found");
        return nullptr;
    }
    if (inet_pton(AF_INET, record->IP().c_str(), &serv_addr.sin_addr) <= 0) {
        Log::Error("invalid address or address not supported");
        return nullptr;
    }

    int rc = ::connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (0 != rc) {
        // connect failed
        return nullptr;
    }

    SecureSocketConfig config;
    config.hostname = d_hostname;
    config.port = d_port;

    // TODO make configurable
    return std::make_shared<SecureSocket>(fd, config);
}

SecureCallbacks::SecureCallbacks(int fd) : d_fd(fd), d_peer_closed(false), d_readWaiting(0)
{
    memset(d_readBuffer, 0, sizeof(d_readBuffer));
}

void SecureCallbacks::readWaiting(uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesRead)
{
    bytesRead = 0;
    if (d_readWaiting > 0) {
        bytesRead = std::min(bufferSize, d_readWaiting);
        memcpy(buffer, d_readBuffer, bytesRead);
        d_readWaiting -= bytesRead;
    }
}

void SecureCallbacks::tls_emit_data(std::span<const uint8_t> data)
{
    int rc = 0;
    uint32_t bytesWritten = 0;
    bytesWritten = 0;
    const uint32_t total = data.size();
    while (bytesWritten < total) {
        rc = ::write(d_fd, data.data() + bytesWritten, total - bytesWritten);
        if (rc < 0) {
            // TODO how to handle errors?
            fwoop::Log::Error("tls write failed: ", std::strerror(errno));
            return;
        }
        bytesWritten += rc;
    }
    Log::Debug("tls wrote ", bytesWritten, " bytes, total=", total);
}

void SecureCallbacks::tls_record_received(uint64_t seqNum, std::span<const uint8_t> data)
{
    // TODO we could recv more data than the read buffer size?
    Log::Info("got app data [seqNum ", seqNum, "]: ", data.size(), " bytes");
    uint32_t n =
        sizeof(d_readBuffer) > d_readWaiting + data.size() ? data.size() : sizeof(d_readBuffer) - d_readWaiting;
    memcpy(d_readBuffer + d_readWaiting, data.data(), n);
    d_readWaiting += data.size();
}

void SecureCallbacks::tls_alert(Botan::TLS::Alert alert)
{
    // TODO should anything else be done?
    auto d = alert.serialize();
    fwoop::Log::Error("TLS error: ", std::string(d.data(), d.data() + d.size()));
}

void SecureCallbacks::tls_session_established(const Botan::TLS::Session_Summary &session)
{
    fwoop::Log::Info("handshake complete, ", session.version().to_string(), " using ",
                     session.ciphersuite().to_string());
}

} // namespace fwoop
