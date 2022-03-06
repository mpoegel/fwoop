#pragma once

#include <fwoop_httpframe.h>

namespace fwoop {

class HttpWindowUpdateFrame : public HttpFrame {
  private:

  public:
    HttpWindowUpdateFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    ~HttpWindowUpdateFrame();

    uint8_t *encode() const override;
};

inline
HttpWindowUpdateFrame::~HttpWindowUpdateFrame()
{
}

}
