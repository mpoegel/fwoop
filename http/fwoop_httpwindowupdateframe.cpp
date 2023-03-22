#include <fwoop_httpwindowupdateframe.h>

namespace fwoop {

HttpWindowUpdateFrame::HttpWindowUpdateFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
    : HttpFrame(length, HttpFrame::Type::WindowUpdate, flags, streamID, payload)
{
}

uint8_t *HttpWindowUpdateFrame::encode() const
{
    uint8_t *encoding = new uint8_t[encodingLength()];
    encodeHeader(encoding);
    // TODO encode window params
    return encoding;
}

} // namespace fwoop
