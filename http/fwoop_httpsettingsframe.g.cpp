#include <fwoop_httpsettingsframe.h>

#include <gtest/gtest.h>

TEST(HttpSettingsFrame, Encode)
{
    // GIVEN
    auto frame = fwoop::HttpSettingsFrame();
    frame.setAck();
    frame.setHeaderTableSize(32000);
    frame.setMaxFrameSize(24);
    const uint8_t expected[] = {0, 0, 12, 4, 1, 0, 0, 0, 0, 0, 1, 0, 0, 125, 0, 0, 6, 0, 0, 0, 24};

    // WHEN
    u_int8_t *encoding = frame.encode();

    // THEN
    ASSERT_EQ(sizeof(expected), frame.encodingLength());

    for (unsigned int i = 0; i < frame.encodingLength(); ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
}

TEST(HttpSettingsFrame, DecodeThenEncode)
{
    // GIVEN
    const uint8_t expected[] = {0, 0, 12, 4, 1, 0, 0, 0, 0, 0, 1, 0, 0, 125, 0, 0, 6, 0, 0, 0, 24};
    const unsigned int length = 12;
    const uint8_t flags = 1;
    auto frame = fwoop::HttpSettingsFrame(length, flags, (uint8_t*)expected + 5, (uint8_t*)expected + 9);

    // WHEN
    u_int8_t *encoding = frame.encode();

    // THEN
    ASSERT_EQ(sizeof(expected), frame.encodingLength());

    for (unsigned int i = 0; i < frame.encodingLength(); ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    frame.printHeader();
    frame.printHex();
}
