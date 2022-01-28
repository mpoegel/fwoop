#include <fwoop_httpframe.h>

#include <cstring>
#include <iostream>

namespace fwoop {

HttpFrame::Type HttpFrame::byteToType(uint8_t byte) {
    switch (byte) {
        case 4:
            return Type::Settings;
        default:
            return Type::Unknown;
    }
}

std::string HttpFrame::typeToString(Type type)
{
    switch (type) {
        case Type::Settings:
            return "Settings";
        default:
            return "Unknown";
    }
}

std::unique_ptr<HttpFrame> HttpFrame::parse(uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesParsed)
{
    bytesParsed = 0;

    if (bufferSize < 9) {
        std::cerr << "frame is too small, size=" << bufferSize << '\n';
        return nullptr;
    }
    unsigned int length = buffer[2] + (buffer[1] >> 8) + (buffer[0] >> 16);
    auto type = HttpFrame::byteToType(buffer[3]);
    uint8_t flags = buffer[4];
    return std::make_unique<HttpFrame>(length, type, flags, buffer + 5, buffer + 9);
}

HttpFrame::HttpFrame(unsigned int length, Type type, uint8_t flags, uint8_t *streamID, uint8_t *payload)
: d_length(length)
, d_type(type)
, d_flags(flags)
{
    d_payload = new uint8_t[d_length];
    memcpy(d_payload, payload, d_length);
    memcpy(d_streamID, streamID, 4);
}

}
