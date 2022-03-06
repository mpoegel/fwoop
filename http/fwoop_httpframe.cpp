#include <fwoop_httpframe.h>
#include <fwoop_httpgoawayframe.h>
#include <fwoop_httpheadersframe.h>
#include <fwoop_httpsettingsframe.h>
#include <fwoop_httpwindowupdateframe.h>

#include <cstring>
#include <iostream>

namespace fwoop {

HttpFrame::Type HttpFrame::byteToType(uint8_t byte) {
    switch (byte) {
        case 0:
            return Type::Data;
        case 1:
            return Type::Header;
        case 2:
            return Type::Priority;
        case 3:
            return Type::ResetStream;
        case 4:
            return Type::Settings;
        case 5:
            return Type::PushPromise;
        case 6:
            return Type::Ping;
        case 7:
            return Type::GoAway;
        case 8:
            return Type::WindowUpdate;
        case 9:
            return Type::Continuation;
        default:
            std::cerr << "unknown frame type: " << (int)byte << '\n';
            return Type::Unknown;
    }
}

std::string HttpFrame::typeToString(Type type)
{
    switch (type) {
        case Type::Data:
            return "Data";
        case Type::Header:
            return "Header";
        case Type::Priority:
            return "Priority";
        case Type::ResetStream:
            return "ResetStream";
        case Type::Settings:
            return "Settings";
        case Type::PushPromise:
            return "PushPromise";
        case Type::Ping:
            return "Ping";
        case Type::GoAway:
            return "GoAway";
        case Type::WindowUpdate:
            return "WindowUpdate";
        case Type::Continuation:
            return "Continuation";
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
    unsigned int length = buffer[2] + (buffer[1] << 8) + (buffer[0] << 16);
    if (length + HEADER_LENGTH <= bufferSize) {
        bytesParsed = length + HEADER_LENGTH;
    } else {
        std::cerr << "buffer does not hold complete frame, length=" << length << ", bufferSize=" << bufferSize << '\n';
        return nullptr;
    }
    auto type = HttpFrame::byteToType(buffer[3]);
    uint8_t flags = buffer[4];
    switch (type) {
        case Type::Header:
            return std::make_unique<HttpHeadersFrame>(length, flags, buffer + 5, buffer + 9);
        case Type::Settings:
            return std::make_unique<HttpSettingsFrame>(length, flags, buffer + 5, buffer + 9);
        case Type::GoAway:
            return std::make_unique<HttpGoAwayFrame>(length, flags, buffer + 5, buffer + 9);
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
    uint8_t *encoding = encode();
    const unsigned int length = encodingLength();
    char buf[3];
    for (unsigned int i = 0; i < length; ++i) {
        if (i == HEADER_LENGTH) {
            std::cout << "| ";
        }
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%x", encoding[i]);
        std::cout << buf << " ";
    }
    std::cout << '\n';
}

void HttpFrame::printHeader() const
{
    std::cout << "length=" << d_length << " type=" << typeToString(d_type) << " flags=" << (int)d_flags << " streamID=" << getStreamID() << "\n";
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
