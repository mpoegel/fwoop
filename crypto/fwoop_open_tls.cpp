#include <fwoop_tls.h>

namespace fwoop {

TLS::TLS(const std::string &certPath, const std::string &keyPath)
    : d_certificatePath(certPath), d_privateKeyPath(keyPath)
{
}

TLS::~TLS() {}

std::error_code TLS::handshake()
{
    d_ctx = std::make_unique<SSL_CTX>(SSL_CTX_new(TLS_method()));
    SSL_CTX_set_min_proto_version(d_ctx.get(), TLS1_2_VERSION);
    return std::error_code();
}

std::error_code TLS::write() { return std::error_code(); }

std::error_code TLS::read() { return std::error_code(); }

} // namespace fwoop
