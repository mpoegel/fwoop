#include <fwoop_httprequest.h>

#include <gtest/gtest.h>

TEST(HttpRequest, encode)
{
    // GIVEN
    fwoop::HttpRequest request;
    request.setMethod(fwoop::HttpRequest::Method::Get);
    request.setPath("/foo");
    request.addHeader(fwoop::HttpHeader::ContentType, "application/json");
    uint32_t length;
    const std::string expected = "GET /foo HTTP/1.1\r\ncontent-type: application/json\r\n\r\n";

    // WHEN
    uint8_t *encoding = request.encode(length);

    // THEN
    ASSERT_EQ(expected.length(), length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    const std::string result((char*)encoding, length);
    EXPECT_EQ(expected, result);

    delete[] encoding;
}
