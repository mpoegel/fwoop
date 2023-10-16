#include <fwoop_tlscredentials.h>

namespace fwoop {
namespace tls {

ClientCredentials::~ClientCredentials()
{
    for (auto store : d_trustedStore) {
        delete store;
    }
}

void ClientCredentials::loadTrustedStoreFromFile(const std::string &dir)
{
    Botan::Certificate_Store *store = new Botan::Certificate_Store_In_Memory(dir);
    d_trustedStore.push_back(store);
}

std::vector<Botan::Certificate_Store *> ClientCredentials::trusted_certificate_authorities(const std::string &type,
                                                                                           const std::string &context)
{
    return d_trustedStore;
}
std::shared_ptr<Botan::Private_Key> ClientCredentials::private_key_for(const Botan::X509_Certificate &cert,
                                                                       const std::string &type,
                                                                       const std::string &context)
{
    return nullptr;
}

} // namespace tls
} // namespace fwoop
