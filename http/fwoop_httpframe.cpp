#include <fwoop_httpframe.h>
#include <fwoop_httpsettingsframe.h>
#include <fwoop_httpwindowupdateframe.h>

#include <cstring>
#include <iostream>

namespace fwoop {

HttpFrame::Type HttpFrame::byteToType(uint8_t byte) {
    switch (byte) {
        case 4:
            return Type::Settings;
        case 7:
            return Type::GoAway;
        case 8:
            return Type::WindowUpdate;
        default:
            std::cerr << "unknown frame type: " << (int)byte << '\n';
            return Type::Unknown;
    }
}

std::string HttpFrame::typeToString(Type type)
{
    switch (type) {
        case Type::Settings:
            return "Settings";
        case Type::GoAway:
            return "GoAway";
        case Type::WindowUpdate:
            return "WindowUpdate";
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
    switch (type) {
        case Type::Settings:
            return std::make_unique<HttpSettingsFrame>(length, flags, buffer + 5, buffer + 9);
        case Type::WindowUpdate:
            return std::make_unique<HttpWindowUpdateFrame>(length, flags, buffer + 5, buffer + 9);
        default:
            return std::make_unique<HttpFrame>(length, type, flags, buffer + 5, buffer + 9);
    }
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

HttpFrame::HttpFrame(Type type)
: d_length(0)
, d_type(type)
, d_flags(0)
, d_payload(nullptr)
{
    memset(d_streamID, 0, 4);
}

void HttpFrame::printHex() const
{
    unsigned int totalLen = (9 + d_length) * 2;
    static constexpr unsigned int HEADER_LEN = 9 * 2 + 3;
    char headerBuf[HEADER_LEN];
    memset(headerBuf, 0, HEADER_LEN);
    sprintf(headerBuf, "%x %x %x %x %x %x %x", d_length, (uint8_t)d_type, d_flags, d_streamID[0], d_streamID[1], d_streamID[2], d_streamID[3]);
    std::cout << "frame: " << headerBuf << " | ";
    char buf[2];
    for (unsigned int i=0; i<d_length; ++i) {
        sprintf(buf, "%x", d_payload[i]);
        std::cout << buf << " ";
    }
    std::cout << '\n';
}

void HttpFrame::encodeHeader(uint8_t *out) const
{
    out[0] = (d_length << 16);
    out[1] = (d_length << 8);
    out[2] = d_length;

    out[3] = (uint8_t)d_type;
    out[4] = d_flags;

    memcpy(out + 5, d_streamID, 4);
}

}
