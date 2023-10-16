#pragma once

#include <fwoop_socketio.h>
#include <fwoop_tlscredentials.h>

#include <cstdint>
#include <memory>
#include <string>
#include <system_error>

#include <botan/auto_rng.h>
#include <botan/certstor.h>
#include <botan/certstor_system.h>
#include <botan/tls.h>
#include <botan/tls_policy.h>
#include <botan/tls_server_info.h>
#include <botan/tls_session_manager_memory.h>
#include <botan/tls_version.h>

namespace fwoop {

class SecureSocket : public SocketBase,
                     public Botan::TLS::Callbacks,
                     public std::enable_shared_from_this<SecureSocket> {
  private:
    int d_fd;
    bool d_peer_closed;
    std::shared_ptr<Botan::TLS::Client> d_client;
    uint8_t d_readBuffer[16384];
    uint32_t d_readWaiting;

    SecureSocket(int fd);

  public:
    [[nodiscard]] static std::shared_ptr<SecureSocket>
    create(int fd, const std::shared_ptr<tls::ClientCredentials> &creds,
           const std::shared_ptr<Botan::AutoSeeded_RNG> &rng,
           const std::shared_ptr<Botan::TLS::Session_Manager_In_Memory> &sessionMgr,
           const Botan::TLS::Server_Information &serverInfo, const std::shared_ptr<Botan::TLS::Policy> &policy,
           Botan::TLS::Protocol_Version version);
    ~SecureSocket();

    std::error_code handshake();

    // from SocketBase
    std::error_code read(uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesRead) override;
    std::error_code write(const uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesWritten) override;
    void close() override;

    // from Botan::TLS::Callbacks
    void tls_emit_data(std::span<const uint8_t> data) override;
    void tls_record_received(uint64_t seq_no, std::span<const uint8_t> data) override;
    void tls_alert(Botan::TLS::Alert alert) override;
    void tls_session_established(const Botan::TLS::Session_Summary &session) override;
    bool tls_peer_closed_connection() override;
};

typedef std::shared_ptr<SecureSocket> SecureSocketPtr_t;

inline bool SecureSocket::tls_peer_closed_connection() { return d_peer_closed; }

class SecureSocketFactory : public SocketFactoryBase {
  private:
    const std::string d_hostname;
    const uint16_t d_port;
    std::shared_ptr<tls::ClientCredentials> d_creds;
    std::shared_ptr<Botan::AutoSeeded_RNG> d_rng;
    std::shared_ptr<Botan::TLS::Session_Manager_In_Memory> d_sessionMgr;
    std::string d_certificatePath;
    std::string d_privateKeyPath;

  public:
    SecureSocketFactory(const std::string &hostname, uint16_t port);
    ~SecureSocketFactory();

    void addTrustStore(const std::string &caPath);
    void setCertificatePath(const std::string &certPath);
    void setPrivateKeyPath(const std::string &keyPath);

    SocketBasePtr_t connect() override;
};

inline void SecureSocketFactory::addTrustStore(const std::string &caPath) { d_creds->loadTrustedStoreFromFile(caPath); }
inline void SecureSocketFactory::setCertificatePath(const std::string &certPath) { d_certificatePath = certPath; }
inline void SecureSocketFactory::setPrivateKeyPath(const std::string &keyPath) { d_privateKeyPath = keyPath; }

} // namespace fwoop
