#pragma once

#include <fwoop_socketio.h>

#include <system_error>

#include <botan/credentials_manager.h>
#include <botan/x509cert.h>

namespace fwoop {
namespace tls {

class ClientCredentials : public Botan::Credentials_Manager {
  private:
    std::vector<Botan::Certificate_Store *> d_trustedStore;

  public:
    ~ClientCredentials();

    void loadTrustedStoreFromFile(const std::string &dir);

    std::vector<Botan::Certificate_Store *> trusted_certificate_authorities(const std::string &type,
                                                                            const std::string &context) override;
    std::shared_ptr<Botan::Private_Key> private_key_for(const Botan::X509_Certificate &cert, const std::string &type,
                                                        const std::string &context) override;
};

} // namespace tls
} // namespace fwoop
