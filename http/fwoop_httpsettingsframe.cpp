#include <fwoop_httpsettingsframe.h>

#include <iostream>

namespace fwoop {

HttpSettingsFrame::HttpSettingsFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
: HttpFrame(length, HttpFrame::Type::Settings, flags, streamID, payload)
{
    if (d_length % 6 != 0) {
        std::cerr << "payload length is not a multiple of 6, length=" << d_length << '\n';
    }

    for (unsigned int i = 0; i <= d_length - PARAMETER_SIZE; i += PARAMETER_SIZE) {
        uint16_t rawParam = (payload[i] << 8) + payload[i + 1];
        uint32_t value = (payload[i+2] << 24) + (payload[i+3] << 16) + (payload[i+4] << 8) + payload[i+5];
        switch (rawParam) {
            case Parameter::HeaderTableSize:
                d_headerTableSize.emplace(value);
                break;
            case Parameter::EnablePush:
                d_enablePush.emplace(value);
                break;
            case Parameter::MaxConcurrentStreams:
                d_maxConcurrentStreams.emplace(value);
                break;
            case Parameter::InitialWindowSize:
                d_initialWindowSize.emplace(value);
                break;
            case Parameter::MaxWindowSize:
                d_maxWindowSize.emplace(value);
                break;
            case Parameter::MaxFrameSize:
                d_maxFrameSize.emplace(value);
                break;
            case Parameter::MaxHeaderListSize:
                d_maxHeaderListSize.emplace(value);
                break;
            default:
                char typeHex[3];
                sprintf(typeHex, "%x", rawParam);
                std::cerr << "invalid setting parameter at " << i << ": 0x" << typeHex << '\n';
                break;
        }
    }
}

HttpSettingsFrame::HttpSettingsFrame()
: HttpFrame(HttpFrame::Type::Settings)
{
}

uint8_t *HttpSettingsFrame::encode() const
{
    uint8_t *encoding = new uint8_t[encodingLength()];
    encodeHeader(encoding);
    unsigned int offset = HEADER_LENGTH;
    if (d_headerTableSize.has_value()) {
        encoding[offset] = uint8_t(HeaderTableSize >> 8);
        encoding[offset + 1] = HeaderTableSize;
        encoding[offset + 2] = (d_headerTableSize.value() >> 24);
        encoding[offset + 3] = (d_headerTableSize.value() >> 16);
        encoding[offset + 4] = (d_headerTableSize.value() >> 8);
        encoding[offset + 5] = d_headerTableSize.value();
        offset += 6;
    }
    if (d_enablePush.has_value()) {
        encoding[offset] = uint8_t(EnablePush >> 8);
        encoding[offset + 1] = EnablePush;
        encoding[offset + 2] = 0;
        encoding[offset + 3] = 0;
        encoding[offset + 4] = 0;
        encoding[offset + 5] = d_enablePush.value();
        offset += 6;
    }
    if (d_maxConcurrentStreams.has_value()) {
        encoding[offset] = uint8_t(MaxConcurrentStreams >> 8);
        encoding[offset + 1] = MaxConcurrentStreams;
        encoding[offset + 2] = d_maxConcurrentStreams.value() >> 24;
        encoding[offset + 3] = d_maxConcurrentStreams.value() >> 16;
        encoding[offset + 4] = d_maxConcurrentStreams.value() >> 8;
        encoding[offset + 5] = d_maxConcurrentStreams.value();
        offset += 6;
    }
    if (d_initialWindowSize.has_value()) {
        encoding[offset] = uint8_t(InitialWindowSize >> 8);
        encoding[offset + 1] = InitialWindowSize;
        encoding[offset + 2] = d_initialWindowSize.value() >> 24;
        encoding[offset + 3] = d_initialWindowSize.value() >> 16;
        encoding[offset + 4] = d_initialWindowSize.value() >> 8;
        encoding[offset + 5] = d_initialWindowSize.value();
        offset += 6;
    }
    if (d_maxWindowSize.has_value()) {
        encoding[offset] = uint8_t(MaxWindowSize >> 8);
        encoding[offset + 1] = MaxWindowSize;
        encoding[offset + 2] = d_maxWindowSize.value() >> 24;
        encoding[offset + 3] = d_maxWindowSize.value() >> 16;
        encoding[offset + 4] = d_maxWindowSize.value() >> 8;
        encoding[offset + 5] = d_maxWindowSize.value();
        offset += 6;
    }

    if (d_maxFrameSize.has_value()) {
        encoding[offset] = uint8_t(MaxFrameSize >> 8);
        encoding[offset + 1] = MaxFrameSize;
        encoding[offset + 2] = d_maxFrameSize.value() >> 24;
        encoding[offset + 3] = d_maxFrameSize.value() >> 16;
        encoding[offset + 4] = d_maxFrameSize.value() >> 8;
        encoding[offset + 5] = d_maxFrameSize.value();
        offset += 6;
    }

    if (d_maxHeaderListSize.has_value()) {
        encoding[offset] = uint8_t(MaxHeaderListSize >> 8);
        encoding[offset + 1] = MaxHeaderListSize;
        encoding[offset + 2] = d_maxHeaderListSize.value() >> 24;
        encoding[offset + 3] = d_maxHeaderListSize.value() >> 16;
        encoding[offset + 4] = d_maxHeaderListSize.value() >> 8;
        encoding[offset + 5] = d_maxHeaderListSize.value();
        offset += 6;
    }
    return encoding;
}

}
