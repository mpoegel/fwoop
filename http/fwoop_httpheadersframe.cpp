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

    if (d_padLength > 0) {
        encoding[HEADER_LENGTH] = d_padLength;
    }

    if (d_streamDep > 0) {
        encoding[HEADER_LENGTH + 1] = (d_streamDep >> 24) | d_isDepMutuallyExclusive;
        encoding[HEADER_LENGTH + 2] = (d_streamDep >> 16);
        encoding[HEADER_LENGTH + 3] = (d_streamDep >> 8);
        encoding[HEADER_LENGTH + 4] = d_streamDep;
    }

    if (d_weight > 0) {
        encoding[HEADER_LENGTH + 5] = d_weight;
    }

    uint32_t compressedLength = d_packer->getEncodedLength(d_headerList);
    uint8_t *compressedHeaders = d_packer->encode(d_headerList);
    memcpy(encoding + HEADER_LENGTH, compressedHeaders, compressedLength);

    return encoding;
}

void HttpHeadersFrame::addHeaderBlock(HttpHeader&& name, std::string&& value)
{
    std::pair<HttpHeaderVariant_t, std::string> pair = std::make_pair<HttpHeaderVariant_t, std::string>(std::move(name), std::move(value));
    d_headerList.push_back(pair);
    d_length = d_packer->getEncodedLength(d_headerList) + countExtraBits();
}

void HttpHeadersFrame::addHeaderBlock(const std::string &name, const std::string& value)
{
    HttpHeaderVariant_t headerName(name);
    d_headerList.emplace_back(headerName, value);
    d_length = d_packer->getEncodedLength(d_headerList) + countExtraBits();
}

uint8_t HttpHeadersFrame::countExtraBits() const
{
    uint8_t extra = 0;
    if (d_padLength > 0) {
        ++extra;
    }
    if (d_streamDep > 0) {
        extra += 4;
    }
    if (d_weight > 0) {
        ++extra;
    }
    return extra;
}

}
