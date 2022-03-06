#pragma once

#include <fwoop_httpframe.h>

namespace fwoop {

class HttpDataFrame : public HttpFrame {
  private:
    uint8_t d_padLength;

  public:
    enum Flag {
        EndStream  = 1,
        Padded     = 8,
    };

    HttpDataFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    explicit HttpDataFrame();

    ~HttpDataFrame();

    uint8_t *encode() const override;

    void addFlag(Flag flag);
    void setData(uint8_t *data, uint32_t dataLen);
};

inline
HttpDataFrame::~HttpDataFrame()
{
}

inline
void HttpDataFrame::addFlag(Flag flag)
{
    d_flags |= flag;
}

}
