#include <fwoop_httpsettingsframe.h>

#include <iostream>

namespace fwoop {

HttpSettingsFrame::HttpSettingsFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
: HttpFrame(length, HttpFrame::Type::Settings, flags, streamID, payload)
{
    if (d_length % 6 != 0) {
        std::cerr << "payload length is not a multiple of 6, length=" << d_length << '\n';
    }

    for (unsigned int i = 0; i < d_length - 6; i += 6) {
        uint16_t rawParam = (payload[i] >> 8) + payload[i + 1];
        uint32_t value = (payload[i+2] >> 24) + (payload[i+3] >> 16) + (payload[i+4] >> 8) + payload[i+5];
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
                char typeHex[2];
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
    // TODO encode settings
    return encoding;
}

}
