#include <fwoop_httpresponse.h>

#include <gtest/gtest.h>

TEST(HttpResponse, encode)
{
    // GIVEN
    fwoop::HttpResponse response;
    response.setStatus("200 OK");
    response.addHeader(fwoop::HttpHeader::ContentType, "application/json");
    uint32_t length;
    const std::string expected = "HTTP/1.1 200 OK\r\ncontent-type: application/json\r\n\r\n";
    
    // WHEN
    uint8_t *encoding = response.encode(length);

    // THEN
    ASSERT_EQ(expected.length(), length);
    for (unsigned int i = 0; i < length; ++i) {
        EXPECT_EQ(expected[i], encoding[i]) << "diff at index=" << i;
    }
    const std::string result((char*)encoding, length);
    EXPECT_EQ(expected, result);

    delete[] encoding;
}
