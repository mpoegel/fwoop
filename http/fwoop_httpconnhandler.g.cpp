#include "fwoop_socketio.h"
#include "gmock/gmock.h"
#include <cstdint>
#include <cstring>
#include <fwoop_httpconnhandler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <system_error>

class MockHttpReader : public fwoop::Reader {
  public:
    ~MockHttpReader() {}
    std::string response;
    std::error_code read(uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesRead)
    {
        ::memcpy(buffer, response.data(), response.length());
        bytesRead = response.length();
        return std::error_code();
    }
    MOCK_METHOD(void, close, ());
};

class MockHttpWriter : public fwoop::Writer {
  public:
    ~MockHttpWriter() {}
    MOCK_METHOD(std::error_code, write, (const uint8_t *buffer, uint32_t bufferSize, uint32_t &bytesWritten));
    MOCK_METHOD(void, close, ());
};

class MockHttpRequestCallback : public fwoop::HttpRequestCallback {
  public:
    ~MockHttpRequestCallback() {}
    MOCK_METHOD(void, onRequest, (const std::shared_ptr<fwoop::HttpRequest> &request, fwoop::HttpResponse &response));
    MOCK_METHOD(void, afterResponse, (const std::shared_ptr<fwoop::HttpRequest> &req, fwoop::WriterPtr_t writer));
};

TEST(HttpConnHandler, Handle)
{
    // GIVEN
    auto reader = std::make_shared<MockHttpReader>();
    reader->response = "HTTP/1.1 GET /\r\n\r\n";
    auto writer = std::make_shared<MockHttpWriter>();
    MockHttpRequestCallback mockCallback;
    fwoop::HttpConnHandler handler(reader, writer, &mockCallback);

    EXPECT_CALL(*reader, close()).WillOnce(::testing::Return());
    EXPECT_CALL(*writer, close()).WillOnce(::testing::Return());
    EXPECT_CALL(*writer, write(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(std::error_code()));
    EXPECT_CALL(mockCallback, onRequest(::testing::_, ::testing::_)).WillOnce(::testing::Return());
    EXPECT_CALL(mockCallback, afterResponse(::testing::_, ::testing::_)).WillOnce(::testing::Return());

    // WHEN
    handler();

    // THEN
}
