#pragma once

#include <fwoop_httpframe.h>

#include <optional>

namespace fwoop {

class HttpSettingsFrame : public HttpFrame {
  private:
    enum Parameter {
        HeaderTableSize      = 1,
        EnablePush           = 2,
        MaxConcurrentStreams = 3,
        InitialWindowSize    = 4,
        MaxWindowSize        = 5,
        MaxFrameSize         = 6,
        MaxHeaderListSize    = 7,
    };

    std::optional<uint32_t> d_headerTableSize;
    std::optional<bool>     d_enablePush;
    std::optional<uint32_t> d_maxConcurrentStreams;
    std::optional<uint32_t> d_initialWindowSize;
    std::optional<uint32_t> d_maxWindowSize;
    std::optional<uint32_t> d_maxFrameSize;
    std::optional<uint32_t> d_maxHeaderListSize;

  public:
    HttpSettingsFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    explicit HttpSettingsFrame();

    ~HttpSettingsFrame();

    void setAck();

    std::optional<uint32_t> headerTableSize() const;
    bool enablePush() const;
    std::optional<uint32_t> maxConcurrentStreams() const;
    std::optional<uint32_t> initialWindowSize() const;
    std::optional<uint32_t> maxFrameSize() const;
    std::optional<uint32_t> maxHeaderListSize() const;

    unsigned int encodingLength() const override;
    uint8_t *encode() const override;
};

inline
HttpSettingsFrame::~HttpSettingsFrame()
{
}

inline
void HttpSettingsFrame::setAck()
{
    d_flags |= 1;
}

inline
unsigned int HttpSettingsFrame::encodingLength() const
{
    return HEADER_LENGTH + d_length;
}

}
