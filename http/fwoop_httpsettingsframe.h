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

    static const unsigned int PARAMETER_SIZE = 6;

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
    void setHeaderTableSize(uint32_t headerTableSize);
    void setEnablePush(bool enablePush);
    void setMaxConcurrentStreams(uint32_t maxConcurrentStreams);
    void setInitialWindowSize(uint32_t initialWindowSize);
    void setMaxFrameSize(uint32_t maxFrameSize);
    void setMaxHeaderListSize(uint32_t maxHeaderListSize);

    std::optional<uint32_t> headerTableSize() const;
    bool enablePush() const;
    std::optional<uint32_t> maxConcurrentStreams() const;
    std::optional<uint32_t> initialWindowSize() const;
    std::optional<uint32_t> maxFrameSize() const;
    std::optional<uint32_t> maxHeaderListSize() const;

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
void HttpSettingsFrame::setHeaderTableSize(uint32_t headerTableSize)
{
    if (!d_headerTableSize.has_value()) {
        d_length += 6;
    }
    d_headerTableSize.emplace(headerTableSize);
}

inline
void HttpSettingsFrame::setMaxFrameSize(uint32_t maxFrameSize)
{
    if (!d_maxFrameSize.has_value()) {
        d_length += 6;
    }
    d_maxFrameSize.emplace(maxFrameSize);
}

inline
std::optional<uint32_t> HttpSettingsFrame::headerTableSize() const
{
    return d_headerTableSize;
}

inline
bool HttpSettingsFrame::enablePush() const
{
    return d_enablePush.has_value() ? d_enablePush.value() : true;
}

inline
std::optional<uint32_t> HttpSettingsFrame::maxConcurrentStreams() const
{
    return d_maxConcurrentStreams;
}

inline
std::optional<uint32_t> HttpSettingsFrame::initialWindowSize() const
{
    return d_initialWindowSize;
}

inline
std::optional<uint32_t> HttpSettingsFrame::maxFrameSize() const
{
    return d_maxFrameSize;
}

inline
std::optional<uint32_t> HttpSettingsFrame::maxHeaderListSize() const
{
    return d_maxHeaderListSize;
}


}
