#pragma once

#include <memory>
#include <string>
#include <system_error>

#ifdef USE_OPENSSL
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

namespace fwoop {

class TLS {

private:
    std::string d_certificatePath;
    std::string d_privateKeyPath;

#ifdef USE_OPENSSL
    std::unique_ptr<SSL_CTX> d_ctx;
    std::unique_ptr<BIO>     d_bio;
#endif

public:
    TLS(const std::string& certPath, const std::string& keyPath);
    ~TLS();

    std::error_code handshake();

    std::error_code write();
    std::error_code read();
};

}
