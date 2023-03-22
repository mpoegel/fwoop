#include <fwoop_sha.h>

#include <iostream>

namespace fwoop {

int SHA::h256(const uint8_t *msg, uint32_t length, uint8_t *digest) { return 0; }

int SHA::h256(const std::string &msg, uint8_t *digest) { return h256((uint8_t *)msg.data(), msg.length(), digest); }

int SHA::SHA256Context::SHA256Reset() { return 0; }

int SHA::SHA256Context::SHA256Input(const uint8_t *bytes, uint32_t length) { return 0; }

int SHA::SHA256Context::SHA256FinalBits(uint8_t bits, uint32_t length) { return 0; }

void SHA::SHA256Context::SHA256Finalize(uint8_t padByte) {}

void SHA::SHA256Context::SHA256PadMessage(uint8_t padByte) {}

int SHA::SHA256Context::SHA256Result(uint8_t *digest) { return SHA256ResultN(digest, SHA256HashSize); }

int SHA::SHA256Context::SHA256ResultN(uint8_t *digest, uint32_t hashSize) { return 0; }

int SHA::SHA256Context::SHA256AddLength(uint32_t length) { return 0; }

int SHA::SHA256Context::SHA256ProcessMessageBlock() { return 0; }

} // namespace fwoop
