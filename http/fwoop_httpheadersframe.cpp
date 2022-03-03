#include <fwoop_httpheadersframe.h>

#include <cstring>
#include <utility>

namespace fwoop {

HttpHeadersFrame::HttpHeadersFrame(unsigned int length, uint8_t flags, uint8_t *streamID, uint8_t *payload)
: HttpFrame(length, HttpFrame::Type::Header, flags, streamID, payload)
, d_padLength(0)
, d_isDepMutuallyExclusive(false)
, d_streamDep(0)
, d_weight(0)
{
    // TODO parse payload
}

uint8_t *HttpHeadersFrame::encode() const
{
    uint8_t *encoding = new uint8_t[encodingLength()];
    encodeHeader(encoding);

    encoding[HEADER_LENGTH] = d_padLength;
    encoding[HEADER_LENGTH + 1] = (d_streamDep >> 24) | d_isDepMutuallyExclusive;
    encoding[HEADER_LENGTH + 2] = (d_streamDep >> 16);
    encoding[HEADER_LENGTH + 3] = (d_streamDep >> 8);
    encoding[HEADER_LENGTH + 4] = d_streamDep;  
    encoding[HEADER_LENGTH + 5] = d_weight;

    uint32_t compressedLength = d_packer->getEncodedLength(d_headerList);
    uint8_t *compressedHeaders = d_packer->encode(d_headerList);
    memcpy(encoding + HEADER_LENGTH + 6, compressedHeaders, compressedLength);

    return encoding;
}

void HttpHeadersFrame::addHeaderBlock(HttpHeader&& name, std::string&& value)
{
    std::pair<HttpHeaderVariant_t, std::string> pair = std::make_pair<HttpHeaderVariant_t, std::string>(std::move(name), std::move(value));
    d_headerList.push_back(pair);
    d_length = 6 + d_packer->getEncodedLength(d_headerList);
}

void HttpHeadersFrame::addHeaderBlock(const std::string &name, const std::string& value)
{
    HttpHeaderVariant_t headerName(name);
    d_headerList.emplace_back(headerName, value);
    d_length = 6 + d_packer->getEncodedLength(d_headerList);
}

}
