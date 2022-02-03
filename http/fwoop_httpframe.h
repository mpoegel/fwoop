#pragma once

#include <memory>
#include <string>

#include <stdint.h>

namespace fwoop {

class HttpFrame {
  public:
    enum class Type {
        Unknown      = 255,
        Settings     =   4,
        GoAway       =   7,
        WindowUpdate =   8,
    };
    static constexpr unsigned int HEADER_LENGTH = 9;

  protected:
    unsigned int  d_length;
    Type          d_type;
    uint8_t       d_flags;
    uint8_t       d_streamID[4];
    uint8_t      *d_payload;

    explicit HttpFrame(Type type);

    void encodeHeader(uint8_t *out) const;

  public:
    static std::unique_ptr<HttpFrame> parse(uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesParsed);
    static Type byteToType(uint8_t byte);
    static std::string typeToString(Type type);

    HttpFrame(unsigned int length, Type type, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    virtual ~HttpFrame();

    unsigned int length() const;
    Type type() const;
    uint8_t *payload() const;
    void printHex() const;
    virtual unsigned int encodingLength() const;
    virtual uint8_t *encode() const;
};

inline
HttpFrame::~HttpFrame()
{
    delete d_payload;
}

inline
unsigned int HttpFrame::length() const
{
    return d_length;
}

inline
HttpFrame::Type HttpFrame::type() const
{
    return d_type;
}

inline
uint8_t *HttpFrame::payload() const
{
    return d_payload;
}

inline
unsigned int HttpFrame::encodingLength() const
{
    return 0;
}

inline
uint8_t *HttpFrame::encode() const
{
    return nullptr;
}

}
