#pragma once

#include <fwoop_httpframe.h>
#include <fwoop_httpheader.h>
#include <fwoop_log.h>

#include <string>
#include <vector>

namespace fwoop {

class HttpHeadersFrame : public HttpFrame {
  private:

    uint8_t                        d_padLength;
    bool                           d_isDepMutuallyExclusive;
    uint32_t                       d_streamDep;
    uint8_t                        d_weight;
    std::vector<HttpHeaderField_t> d_headerList;

  public:
    enum Flag {
        EndStream  = 1,
        EndHeaders = 4,
        Padded     = 8,
        Priority   = 32,
    };

    HttpHeadersFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload);
    explicit HttpHeadersFrame();

    void addFlag(Flag flag);

    void addHeaderBlock(HttpHeader&& name, std::string&& value);
    void addHeaderBlock(const std::string &name, const std::string& value);
    void clearHeaderBlocks();

    uint8_t *encode() const override;

    std::vector<HttpHeaderField_t>::const_iterator beginHeaderList() const;
    std::vector<HttpHeaderField_t>::const_iterator endHeaderList() const;
};

inline
HttpHeadersFrame::HttpHeadersFrame()
: HttpFrame(HttpFrame::Type::Header)
, d_padLength(0)
, d_isDepMutuallyExclusive(false)
, d_streamDep(0)
, d_weight(0)
{
    d_length += 6;
}

inline
void HttpHeadersFrame::addFlag(Flag flag)
{
    d_flags |= flag;
}

inline
void HttpHeadersFrame::clearHeaderBlocks()
{
    d_headerList.clear();
}

inline
std::vector<HttpHeaderField_t>::const_iterator HttpHeadersFrame::beginHeaderList() const
{
    return d_headerList.cbegin();
}

inline
std::vector<HttpHeaderField_t>::const_iterator HttpHeadersFrame::endHeaderList() const
{
    return d_headerList.cend();
}

}
