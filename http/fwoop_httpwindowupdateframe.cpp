#include <fwoop_httpwindowupdateframe.h>

namespace fwoop {

HttpWindowUpdateFrame::HttpWindowUpdateFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
: HttpFrame(length, HttpFrame::Type::WindowUpdate, flags, streamID, payload)
{
}

}
