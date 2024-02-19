#pragma once

#include <cstring>
#include <memory>
#include <string>

#include <stdint.h>

namespace fwoop {

class HttpFrame {
  public:
    enum class Type {
        Unknown = 255,
        Data = 0,
        Header = 1,
        Priority = 2,
        ResetStream = 3,
        Settings = 4,
        PushPromise = 5,
        Ping = 6,
        GoAway = 7,
        WindowUpdate = 8,
        Continuation = 9,
    };
    static constexpr unsigned int HEADER_LENGTH = 9;

  protected:
    unsigned int d_length;
    Type d_type;
    uint8_t d_flags;
    uint8_t d_streamID[4];
    uint8_t *d_payload;

    explicit HttpFrame(Type type);

    void encodeHeader(uint8_t *out) const;

  public:
    static std::unique_ptr<HttpFrame> parse(uint8_t *buffer, unsigned int bufferSize, unsigned int &bytesParsed);
    static Type byteToType(uint8_t byte);
    static std::string typeToString(Type type);

    HttpFrame(unsigned int length, Type type, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    virtual ~HttpFrame();

    void setStreamID(uint32_t id);
    void setLength(unsigned int length);

    unsigned int length() const;
    Type type() const;
    uint8_t *payload() const;
    void printHex() const;
    void printHeader() const;
    unsigned int encodingLength() const;
    virtual uint8_t *encode() const;
    uint32_t getStreamID() const;
};

inline HttpFrame::~HttpFrame() { delete[] d_payload; }

inline unsigned int HttpFrame::length() const { return d_length; }

inline HttpFrame::Type HttpFrame::type() const { return d_type; }

inline uint8_t *HttpFrame::payload() const { return d_payload; }

inline unsigned int HttpFrame::encodingLength() const { return HEADER_LENGTH + d_length; }

inline uint8_t *HttpFrame::encode() const
{
    uint8_t *encoding = new uint8_t[encodingLength()];
    memset(encoding, 0, encodingLength());
    encodeHeader(encoding);
    return encoding;
}

inline void HttpFrame::setStreamID(uint32_t id)
{
    d_streamID[0] = (id << 24);
    d_streamID[1] = (id << 16);
    d_streamID[2] = (id << 8);
    d_streamID[3] = id;
}

inline uint32_t HttpFrame::getStreamID() const
{
    uint32_t result = d_streamID[3] + (d_streamID[2] << 8) + (d_streamID[1] << 16) + (d_streamID[0] << 24);
    return result;
}

inline void HttpFrame::setLength(unsigned int length) { d_length = length; }

} // namespace fwoop
