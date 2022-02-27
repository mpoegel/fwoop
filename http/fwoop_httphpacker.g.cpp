#include <fwoop_httphpacker.h>

#include <fwoop_httpheadersframe.h>
#include <fwoop_log.h>

#include <gtest/gtest.h>

TEST(HttpHPacker, encodeLength)
{
    // GIVEN
    std::shared_ptr<fwoop::HttpHeadersFrame> frame = std::make_shared<fwoop::HttpHeadersFrame>();
    frame->addHeaderBlock(fwoop::HttpHeader::Method, "get");
    frame->addHeaderBlock(fwoop::HttpHeader::Scheme, "http");
    frame->addHeaderBlock(fwoop::HttpHeader::Path, "/");
    frame->addHeaderBlock(fwoop::HttpHeader::Authority, "www.example.com");

    fwoop::HttpHPacker packer;

    // WHEN
    uint32_t length = packer.encodeLength(frame);

    // THEN
    EXPECT_EQ(20, length);

    // GIVEN
    frame->addHeaderBlock("cache-control", "no-cache");

    // WHEN
    length = packer.encodeLength(frame);

    // THEN
    EXPECT_EQ(14, length);

    // GIVEN
    frame = std::make_shared<fwoop::HttpHeadersFrame>();
    frame->addHeaderBlock(fwoop::HttpHeader::Method, "get");
    frame->addHeaderBlock(fwoop::HttpHeader::Scheme, "http");
    frame->addHeaderBlock(fwoop::HttpHeader::Path, "/");
    frame->addHeaderBlock(fwoop::HttpHeader::Authority, "www.example.com");
    frame->addHeaderBlock("custom-key", "custom-value");

    // WHEN
    length = packer.encodeLength(frame);

    // THEN
    EXPECT_EQ(29, length);
}


TEST(HttpHPacker, encode)
{
    // GIVEN
    std::shared_ptr<fwoop::HttpHeadersFrame> frame = std::make_shared<fwoop::HttpHeadersFrame>();
    frame->addHeaderBlock(fwoop::HttpHeader::Method, "get");
    frame->addHeaderBlock(fwoop::HttpHeader::Scheme, "http");
    frame->addHeaderBlock(fwoop::HttpHeader::Path, "/");
    frame->addHeaderBlock(fwoop::HttpHeader::Authority, "www.example.com");

    fwoop::HttpHPacker packer;
    uint8_t expected1[] = {0x82, 0x86, 0x84, 0x41, 0x0f, 0x77, 0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d};

    // WHEN
    uint32_t length = packer.getEncodedLength(frame);
    u_int8_t *encoding = packer.encodeHeaderFrame(frame);

    // THEN
    ASSERT_EQ(20, length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected1[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;

    // GIVEN
    frame->addHeaderBlock("cache-control", "no-cache");
    uint8_t expected2[] = {0x82, 0x86, 0x84, 0xbe, 0x58, 0x08, 0x6e, 0x6f, 0x2d, 0x63, 0x61, 0x63, 0x68, 0x65};

    // WHEN
    length = packer.getEncodedLength(frame);
    encoding = packer.encodeHeaderFrame(frame);

    // THEN
    ASSERT_EQ(14, length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected2[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;

    // GIVEN
    frame->clearHeaderBlocks();
    frame->addHeaderBlock(fwoop::HttpHeader::Method, "get");
    frame->addHeaderBlock(fwoop::HttpHeader::Scheme, "https");
    frame->addHeaderBlock(fwoop::HttpHeader::Path, "/index.html");
    frame->addHeaderBlock(fwoop::HttpHeader::Authority, "www.example.com");
    frame->addHeaderBlock("custom-key", "custom-value");
    uint8_t expected3[] = {0x82, 0x87, 0x85, 0xbf, 0x40, 0x0a, 0x63, 0x75, 0x73, 0x74, 0x6f, 0x6d, 0x2d, 0x6b, 0x65, 0x79,
        0x0c, 0x63, 0x75, 0x73, 0x74, 0x6f, 0x6d, 0x2d, 0x76, 0x61, 0x6c, 0x75, 0x65};

    // WHEN
    length = packer.getEncodedLength(frame);
    encoding = packer.encodeHeaderFrame(frame);

    // THEN
    EXPECT_EQ(29, length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected3[i], encoding[i]) << "diff at index=" << i;
    }
    delete[] encoding;
}
