#pragma once

#include <fwoop_httpframe.h>

namespace fwoop {

class HttpGoAwayFrame : public HttpFrame {
  private:
    uint32_t d_lastStreamID;
    uint32_t d_errorCode;

  public:
    HttpGoAwayFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    explicit HttpGoAwayFrame();

    ~HttpGoAwayFrame();

    uint8_t *encode() const override;

    void setLastStreamID(uint32_t streamID);
    void setErrorCode(uint32_t ec);
};

inline
HttpGoAwayFrame::~HttpGoAwayFrame()
{
}

inline
void HttpGoAwayFrame::setLastStreamID(uint32_t streamID)
{
    d_lastStreamID = streamID;
}

inline
void HttpGoAwayFrame::setErrorCode(uint32_t ec)
{
    d_errorCode = ec;
}

}
