#include <fwoop_httphpacker.h>

#include <gtest/gtest.h>

TEST(HttpHPacker, encodeLength)
{
    // GIVEN
    std::vector<fwoop::HttpHeaderField_t> headerFields{{fwoop::HttpHeader::Method, "get"},
                                                       {fwoop::HttpHeader::Scheme, "http"},
                                                       {fwoop::HttpHeader::Path, "/"},
                                                       {fwoop::HttpHeader::Authority, "www.example.com"}};

    fwoop::HttpHPacker packer;

    // WHEN
    uint32_t length = packer.encodeLength(headerFields);

    // THEN
    EXPECT_EQ(20, length);

    // GIVEN
    headerFields.push_back({"cache-control", "no-cache"});

    // WHEN
    length = packer.encodeLength(headerFields);

    // THEN
    EXPECT_EQ(14, length);

    // GIVEN
    headerFields = std::vector<fwoop::HttpHeaderField_t>{{fwoop::HttpHeader::Method, "get"},
                                                         {fwoop::HttpHeader::Scheme, "https"},
                                                         {fwoop::HttpHeader::Path, "/index.html"},
                                                         {fwoop::HttpHeader::Authority, "www.example.com"},
                                                         {"custom-key", "custom-value"}};

    // WHEN
    length = packer.encodeLength(headerFields);

    // THEN
    EXPECT_EQ(29, length);
}

TEST(HttpHPacker, encode)
{
    // GIVEN
    std::vector<fwoop::HttpHeaderField_t> headerFields{{fwoop::HttpHeader::Method, "get"},
                                                       {fwoop::HttpHeader::Scheme, "http"},
                                                       {fwoop::HttpHeader::Path, "/"},
                                                       {fwoop::HttpHeader::Authority, "www.example.com"}};

    fwoop::HttpHPacker packer;
    uint8_t expected1[] = {0x82, 0x86, 0x84, 0x41, 0x0f, 0x77, 0x77, 0x77, 0x2e, 0x65,
                           0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d};

    // WHEN
    uint32_t length = packer.getEncodedLength(headerFields);
    u_int8_t *encoding = packer.encode(headerFields);

    // THEN
    ASSERT_EQ(20, length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected1[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;

    // GIVEN
    headerFields.push_back({"cache-control", "no-cache"});
    uint8_t expected2[] = {0x82, 0x86, 0x84, 0xbe, 0x58, 0x08, 0x6e, 0x6f, 0x2d, 0x63, 0x61, 0x63, 0x68, 0x65};

    // WHEN
    length = packer.getEncodedLength(headerFields);
    encoding = packer.encode(headerFields);

    // THEN
    ASSERT_EQ(14, length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected2[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;

    // GIVEN
    headerFields = std::vector<fwoop::HttpHeaderField_t>{{fwoop::HttpHeader::Method, "get"},
                                                         {fwoop::HttpHeader::Scheme, "https"},
                                                         {fwoop::HttpHeader::Path, "/index.html"},
                                                         {fwoop::HttpHeader::Authority, "www.example.com"},
                                                         {"custom-key", "custom-value"}};
    uint8_t expected3[] = {0x82, 0x87, 0x85, 0xbf, 0x40, 0x0a, 0x63, 0x75, 0x73, 0x74, 0x6f, 0x6d, 0x2d, 0x6b, 0x65,
                           0x79, 0x0c, 0x63, 0x75, 0x73, 0x74, 0x6f, 0x6d, 0x2d, 0x76, 0x61, 0x6c, 0x75, 0x65};

    // WHEN
    length = packer.getEncodedLength(headerFields);
    encoding = packer.encode(headerFields);

    // THEN
    EXPECT_EQ(29, length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected3[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;
}
