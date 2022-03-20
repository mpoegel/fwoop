#include <fwoop_sha.h>

#include <iostream>

namespace fwoop {

#define SHA256_SHR(bits, word) ((word) >> (bits))
#define SHA256_ROTL(bits, word) (((word) << (bits)) | ((word) >> (32-(bits))))
#define SHA256_ROTR(bits, word) (((word) >> (bits)) | ((word) << (32-(bits))))

#define SHA256_SIGMA0(word) (SHA256_ROTR(2,  word) ^ SHA256_ROTR(13, word) ^ SHA256_ROTR(22, word))
#define SHA256_SIGMA1(word) (SHA256_ROTR(6,  word) ^ SHA256_ROTR(11, word) ^ SHA256_ROTR(25, word))
#define SHA256_sigma0(word) (SHA256_ROTR(7,  word) ^ SHA256_ROTR(18, word) ^ SHA256_SHR(3,  word))
#define SHA256_sigma1(word) (SHA256_ROTR(17, word) ^ SHA256_ROTR(19, word) ^ SHA256_SHR(10, word))

#define SHA_Ch(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define SHA_Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))


const uint32_t SHA::SHA256_H0[] = {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

int SHA::h256(const uint8_t *msg, uint32_t length, uint8_t *digest)
{
    SHA256Context context;

    context.SHA256Reset();
    if (0 != context.SHA256Input(msg, length)) {
        return 1;
    }
    if (0 != context.SHA256Result(digest)) {
        return 1;
    }
    return 0;
}

int SHA::h256(const std::string& msg, uint8_t *digest)
{
    return h256((uint8_t*)msg.data(), msg.length(), digest);
}

int SHA::SHA256Context::SHA256Reset()
{
    LengthHigh = 0;
    LengthLow = 0;
    MessageBlockIndex = 0;

    IntermediateHash[0] = SHA256_H0[0];
    IntermediateHash[1] = SHA256_H0[1];
    IntermediateHash[2] = SHA256_H0[2];
    IntermediateHash[3] = SHA256_H0[3];
    IntermediateHash[4] = SHA256_H0[4];
    IntermediateHash[5] = SHA256_H0[5];
    IntermediateHash[6] = SHA256_H0[6];
    IntermediateHash[7] = SHA256_H0[7];

    for (uint8_t i = 0; i < SHA256MessageBlockSize; i++) {
        MessageBlock[i] = 0;
    }

    Computed = 0;
    Corrupted = 0;

    return 0;
}

int SHA::SHA256Context::SHA256Input(const uint8_t *bytes, uint32_t length)
{
    if (length == 0) return -1;
    if (bytes == nullptr) return -1;
    if (Computed) return -2;
    if (Corrupted) return -2;

    while (length-- > 0) {
        MessageBlock[MessageBlockIndex++] = *bytes;

        if (SHA256AddLength(8) == 0 && MessageBlockIndex == SHA256MessageBlockSize) {
            SHA256ProcessMessageBlock();
        }

        bytes++;
    }
    return Corrupted;
}

int SHA::SHA256Context::SHA256FinalBits(uint8_t bits, uint32_t length)
{
    static uint8_t masks[8] = {
        /* 0 0b00000000 */ 0x00, /* 1 0b10000000 */ 0x80,
        /* 2 0b11000000 */ 0xC0, /* 3 0b11100000 */ 0xE0,
        /* 4 0b11110000 */ 0xF0, /* 5 0b11111000 */ 0xF8,
        /* 6 0b11111100 */ 0xFC, /* 7 0b11111110 */ 0xFE
    };
    static uint8_t markbit[8] = {
        /* 0 0b10000000 */ 0x80, /* 1 0b01000000 */ 0x40,
        /* 2 0b00100000 */ 0x20, /* 3 0b00010000 */ 0x10,
        /* 4 0b00001000 */ 0x08, /* 5 0b00000100 */ 0x04,
        /* 6 0b00000010 */ 0x02, /* 7 0b00000001 */ 0x01
    };

    if (length == 0) return -1;
    if (Corrupted) return -2;
    if (Computed) return -2;
    if (length >= 8) {
        Corrupted = 2;
        return -1;
    }

    SHA256AddLength(length);
    SHA256Finalize((uint8_t)((bits & masks[length]) | markbit[length]));

    return Corrupted;
}

void SHA::SHA256Context::SHA256Finalize(uint8_t padByte)
{
    SHA256PadMessage(padByte);
    for (uint32_t i = 0; i < SHA256MessageBlockSize; i++) {
        MessageBlock[i] = 0;
    }
    LengthHigh = 0;
    LengthLow = 0;
    Computed = 1;
}

void SHA::SHA256Context::SHA256PadMessage(uint8_t padByte)
{
    if (MessageBlockIndex >= SHA256MessageBlockSize - 8) {
        MessageBlock[MessageBlockIndex++] = padByte;
        while (MessageBlockIndex < SHA256MessageBlockSize) {
            MessageBlock[MessageBlockIndex++] = 0;
        }
        SHA256ProcessMessageBlock();
    } else {
        MessageBlock[MessageBlockIndex++] = padByte;
    }

    while (MessageBlockIndex < SHA256MessageBlockSize - 8) {
        MessageBlock[MessageBlockIndex++] = 0;
    }

    MessageBlock[56] = (uint8_t)(LengthHigh >> 24);
    MessageBlock[57] = (uint8_t)(LengthHigh >> 16);
    MessageBlock[58] = (uint8_t)(LengthHigh >> 8);
    MessageBlock[59] = (uint8_t)(LengthHigh);
    MessageBlock[60] = (uint8_t)(LengthLow >> 24);
    MessageBlock[61] = (uint8_t)(LengthLow >> 16);
    MessageBlock[62] = (uint8_t)(LengthLow >> 8);
    MessageBlock[63] = (uint8_t)(LengthLow);

    SHA256ProcessMessageBlock();
}

int SHA::SHA256Context::SHA256Result(uint8_t *digest)
{
    return SHA256ResultN(digest, SHA256HashSize);
}

int SHA::SHA256Context::SHA256ResultN(uint8_t *digest, uint32_t hashSize)
{
    if (1 != Computed) {
        SHA256Finalize(0x80);
    }

    for (uint32_t i = 0; i < hashSize; i++) {
        digest[i] = (uint8_t)(IntermediateHash[i >> 2] >> 8 * (3 - (i & 0x03)));
    }

    return 0;
}


int SHA::SHA256Context::SHA256AddLength(uint32_t length)
{
    uint32_t addTemp = LengthLow;
    LengthLow += length;
    Corrupted = LengthLow < addTemp && LengthHigh++ == 0
        ? 1 : Corrupted;
    return 0;
}

int SHA::SHA256Context::SHA256ProcessMessageBlock()
{
    static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
        0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
        0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
        0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
        0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
        0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
        0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
        0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
        0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
        0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    uint32_t temp1, temp2;  // temp word value
    uint32_t W[64];  // word sequence
    uint32_t A, B, C, D, E, F, G, H;  // word buffers

    for (uint16_t t = 0, t4 = 0; t < 16; t++, t4 += 4) {
        W[t] = (((uint32_t)MessageBlock[t4]) << 24) |
               (((uint32_t)MessageBlock[t4 + 1]) << 16) |
               (((uint32_t)MessageBlock[t4 + 2]) << 8) |
               (((uint32_t)MessageBlock[t4 + 3]));
    }

    for (uint16_t t = 16; t < 64; t++) {
        W[t] = SHA256_sigma1(W[t-2]) + W[t-7] + SHA256_sigma0(W[t-15]) + W[t-16];
    }

    A = IntermediateHash[0];
    B = IntermediateHash[1];
    C = IntermediateHash[2];
    D = IntermediateHash[3];
    E = IntermediateHash[4];
    F = IntermediateHash[5];
    G = IntermediateHash[6];
    H = IntermediateHash[7];

    for (uint16_t t = 0; t < 64; t++) {
        temp1 = H + SHA256_SIGMA1(E) + SHA_Ch(E, F, G) + K[t] + W[t];
        temp2 = SHA256_SIGMA0(A) + SHA_Maj(A, B, C);
        H = G;
        G = F;
        F = E;
        E = D + temp1;
        D = C;
        C = B;
        B = A;
        A = temp1 + temp2;
    }

    IntermediateHash[0] += A;
    IntermediateHash[1] += B;
    IntermediateHash[2] += C;
    IntermediateHash[3] += D;
    IntermediateHash[4] += E;
    IntermediateHash[5] += F;
    IntermediateHash[6] += G;
    IntermediateHash[7] += H;

    MessageBlockIndex = 0;

    return 0;
}

}
