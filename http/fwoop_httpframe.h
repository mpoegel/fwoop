#pragma once

#include <memory>
#include <string>

#include <stdint.h>

namespace fwoop {

class HttpFrame {
  public:
    enum class Type {
        Unknown  = -1,
        Settings =  4,
    };

  private:
    unsigned int  d_length;
    Type          d_type;
    uint8_t       d_flags;
    uint8_t       d_streamID[4];
    uint8_t      *d_payload;

  public:
    static std::unique_ptr<HttpFrame> parse(uint8_t *buffer, unsigned int bufferSize, unsigned int& bytesParsed);
    static Type byteToType(uint8_t byte);
    static std::string typeToString(Type type);

    HttpFrame(unsigned int length, Type type, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    ~HttpFrame();

    unsigned int length() const;
    Type type() const;
    uint8_t *payload() const;
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

}
