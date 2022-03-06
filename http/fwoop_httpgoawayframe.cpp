#include <fwoop_httpgoawayframe.h>

#include <fwoop_log.h>

namespace fwoop {

HttpGoAwayFrame::HttpGoAwayFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
: HttpFrame(length, HttpFrame::Type::GoAway, flags, streamID, payload)
, d_lastStreamID(0)
, d_errorCode(0)
{
    d_length = 8;
}

HttpGoAwayFrame::HttpGoAwayFrame()
: HttpFrame(HttpFrame::Type::GoAway)
, d_lastStreamID(0)
, d_errorCode(0)
{
    d_length = 8;
}

uint8_t *HttpGoAwayFrame::encode() const
{
    unsigned int length = encodingLength();
    uint8_t *encoding = new uint8_t[length];
    memset(encoding, 0, length);
    encodeHeader(encoding);

    encoding[HttpFrame::HEADER_LENGTH + 0] = (d_lastStreamID << 24);
    encoding[HttpFrame::HEADER_LENGTH + 1] = (d_lastStreamID << 16);
    encoding[HttpFrame::HEADER_LENGTH + 2] = (d_lastStreamID << 8);
    encoding[HttpFrame::HEADER_LENGTH + 3] = d_lastStreamID;

    encoding[HttpFrame::HEADER_LENGTH + 4] = (d_errorCode << 24);
    encoding[HttpFrame::HEADER_LENGTH + 5] = (d_errorCode << 16);
    encoding[HttpFrame::HEADER_LENGTH + 6] = (d_errorCode << 8);
    encoding[HttpFrame::HEADER_LENGTH + 7] = d_errorCode;

    return encoding;
}

}
