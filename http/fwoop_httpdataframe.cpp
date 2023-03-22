#include <fwoop_httpdataframe.h>

#include <fwoop_log.h>

namespace fwoop {

HttpDataFrame::HttpDataFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
    : HttpFrame(length, HttpFrame::Type::Data, flags, streamID, payload), d_padLength(0)
{
}

HttpDataFrame::HttpDataFrame() : HttpFrame(HttpFrame::Type::Data), d_padLength(0) {}

uint8_t *HttpDataFrame::encode() const
{
    unsigned int length = encodingLength();
    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    encodeHeader(encoding);
    encoding[HttpFrame::HEADER_LENGTH] = d_padLength;
    memcpy(encoding + HttpFrame::HEADER_LENGTH + 1, d_payload, d_length - 1 - d_padLength);
    return encoding;
}

void HttpDataFrame::setData(uint8_t *data, uint32_t dataLen)
{
    if (d_payload != nullptr) {
        delete[] d_payload;
    }
    d_payload = new uint8_t[dataLen];
    memcpy(d_payload, data, dataLen);
    d_length = dataLen + 1 + d_padLength;
}

} // namespace fwoop
