#include <fwoop_log.h>
#include <fwoop_sha.h>
#include <gtest/gtest.h>

TEST(Sha, sha256)
{
    // GIVEN
    uint8_t msg[] = "fwoop fwoop fwoop";
    const uint32_t msgLen = strlen((char *)msg);
    const uint8_t expectedHash[] = {0xc4, 0x06, 0x68, 0xd5, 0x4c, 0x00, 0x26, 0xe1, 0x3e, 0xc0, 0xb4,
                                    0x2f, 0xd4, 0x9f, 0x90, 0xfa, 0x72, 0xcf, 0xd4, 0xa2, 0x40, 0x7f,
                                    0x6d, 0xa7, 0xfd, 0x87, 0xd9, 0x8b, 0xe1, 0x36, 0xdf, 0x91};
    uint8_t hash[fwoop::SHA::SHA256HashSize];
    memset(hash, 0, fwoop::SHA::SHA256HashSize);

    // WHEN
    ASSERT_EQ(0, fwoop::SHA::h256(msg, msgLen, hash));

    // THEN
    for (int i = 0; i < fwoop::SHA::SHA256HashSize; i++) {
        EXPECT_EQ(expectedHash[i], hash[i]) << "diff at i=" << i;
    }
}
