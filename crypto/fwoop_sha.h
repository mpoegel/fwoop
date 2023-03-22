#pragma once

#include <stdint.h>
#include <string>

namespace fwoop {

class SHA {
  public:
    static const uint32_t SHA256HashSize = 32;

  private:
    static const uint32_t SHA256MessageBlockSize = 64;
    static const uint32_t SHA256_H0[SHA256HashSize / 4];

    struct SHA256Context {
        uint32_t IntermediateHash[SHA256HashSize / 4];

        uint32_t LengthHigh;
        uint32_t LengthLow;

        int16_t MessageBlockIndex;

        uint8_t MessageBlock[SHA256MessageBlockSize];

        int Computed;
        int Corrupted;

        int SHA256Reset();
        int SHA256Input(const uint8_t *bytes, uint32_t length);
        int SHA256FinalBits(uint8_t bits, uint32_t length);
        int SHA256Result(uint8_t *digest);
        void SHA256Finalize(uint8_t padByte);

        void SHA256PadMessage(uint8_t padByte);
        int SHA256AddLength(uint32_t length);
        int SHA256ProcessMessageBlock();
        int SHA256ResultN(uint8_t *digest, uint32_t hashSize);
    };

  public:
    static int h256(const uint8_t *msg, uint32_t length, uint8_t *digest);
    static int h256(const std::string &msg, uint8_t *digest);
};

} // namespace fwoop
