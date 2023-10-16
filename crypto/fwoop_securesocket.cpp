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
#include <openssl/x509v3.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace fwoop {

SecureSocket::SecureSocket(int fd) : d_fd(fd), d_peer_closed(false), d_readWaiting(0)
{
    memset(d_readBuffer, 0, sizeof(d_readBuffer));
}

SecureSocket::~SecureSocket() {}

[[nodiscard]] std::shared_ptr<SecureSocket>
SecureSocket::create(int fd, const std::shared_ptr<tls::ClientCredentials> &creds,
                     const std::shared_ptr<Botan::AutoSeeded_RNG> &rng,
                     const std::shared_ptr<Botan::TLS::Session_Manager_In_Memory> &sessionMgr,
                     const Botan::TLS::Server_Information &serverInfo,
                     const std::shared_ptr<Botan::TLS::Policy> &policy, Botan::TLS::Protocol_Version version)
{
    auto sock = std::shared_ptr<SecureSocket>(new SecureSocket(fd));
    sock->d_client = std::make_shared<Botan::TLS::Client>(sock->shared_from_this(), sessionMgr, creds, policy, rng,
                                                          serverInfo, version);
    return sock;
};

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
    memset(d_readBuffer, 0, sizeof(d_readBuffer));
    bytesRead = 0;
    if (d_readWaiting == 0) {
        struct pollfd pfd[1];
        pfd[0].fd = d_fd;
        pfd[0].events = POLLIN;

        int rc = poll(pfd, 1, 1000);
        if (rc == 0) {
            // poll timed out
            return std::error_code(errno, std::system_category());
        } else if (rc < 0) {
            // poll failed
            return std::error_code(errno, std::system_category());
        }

        if (pfd[0].revents & POLLIN) {
            rc = ::read(d_fd, buffer, bufferSize);
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
        d_client->received_data(buffer, bytesRead);
    }
    // reset the buffer
    memset(buffer, 0, bufferSize);
    // copy the decrypted data into the buffer
    bytesRead = d_readWaiting >= bufferSize ? bufferSize : d_readWaiting;
    memcpy(buffer, d_readBuffer, bytesRead);
    d_readWaiting = 0;
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

void SecureSocket::tls_emit_data(std::span<const uint8_t> data)
{
    int rc = 0;
    uint32_t bytesWritten = 0;
    bytesWritten = 0;
    while (bytesWritten < data.size()) {
        rc = ::write(d_fd, data.data() + bytesWritten, data.size() - bytesWritten);
        if (rc < 0) {
            // TODO how to handle errors?
            fwoop::Log::Error("tls write failed: ", std::strerror(errno));
        }
        bytesWritten += rc;
    }
    Log::Debug("tls wrote ", bytesWritten, " bytes");
}

void SecureSocket::tls_record_received(uint64_t seq_no, std::span<const uint8_t> data)
{
    // TODO we could recv more data than the read buffer size?
    Log::Info("got app data: ", data.size(), " bytes");
    uint32_t n =
        sizeof(d_readBuffer) > d_readWaiting + data.size() ? data.size() : sizeof(d_readBuffer) - d_readWaiting;
    memcpy(d_readBuffer + d_readWaiting, data.data(), n);
    d_readWaiting = data.size();
}

void SecureSocket::tls_alert(Botan::TLS::Alert alert)
{
    // TODO should anything else be done?
    auto d = alert.serialize();
    fwoop::Log::Error("TLS error: ", std::string(d.data(), d.data() + d.size()));
}

void SecureSocket::tls_session_established(const Botan::TLS::Session_Summary &session)
{
    std::cout << "Handshake complete, " << session.version().to_string() << " using "
              << session.ciphersuite().to_string() << std::endl;
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

    auto record = DNS::Query::getRecord(d_hostname);
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

    // TODO make configurable
    auto policy = std::make_shared<Botan::TLS::Policy>();
    Botan::TLS::Protocol_Version version = Botan::TLS::Protocol_Version::TLS_V12;
    auto info = Botan::TLS::Server_Information(d_hostname, d_port);

    return SecureSocket::create(fd, d_creds, d_rng, d_sessionMgr, info, policy, version);
}

} // namespace fwoop
